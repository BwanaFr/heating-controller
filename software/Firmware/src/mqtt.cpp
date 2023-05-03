#include "mqtt.h"
#include "parameters.h"
#include "process.h"
#include "ui/events.h"
#include <lvgl.h>

#define PROFILE_PREFIX "/profile"
#define STATUS_PREFIX "/status"

MQTT* MQTT::instance_ = nullptr;

MQTT::MQTT() : 
    client_(espClient_), lastPostTime_(0),
    lastConAttempt_(0), mqttState_(MQTTConState::Disconnected)
{
    instance_ = this;
    semaphore_ = xSemaphoreCreateMutex();
}

void MQTT::connectTask(void* parameters)
{
    MQTT* self = static_cast<MQTT*>(parameters);
    //Waits to acquire the semaphore
    while(!xSemaphoreTake(self->semaphore_, 10)){
        yield();
    }
    // Create a Client ID baased on MAC address
    byte mac[6];
    WiFi.macAddress(mac);
    String clientId = "Heater-";
    clientId += String(mac[3], HEX);
    clientId += String(mac[4], HEX);
    clientId += String(mac[5], HEX);

    if(self->client_.connect(clientId.c_str(), Parameters::getInstance()->getMQTTUser().c_str(), Parameters::getInstance()->getMQTTPass().c_str())){
        self->setState(MQTTConState::Connected);
        //Subscribe to MQTT topics
        self->client_.subscribe((self->prefix_ + PROFILE_PREFIX).c_str());
        /*client_.subscribe((prefix_ + "/profile1Set").c_str());
        client_.subscribe((prefix_ + "/profile2Set").c_str());*/
    } else {
        Serial.print("failed, rc=");
        Serial.print(self->client_.state());
        Serial.println(" try again in 5 seconds");
        self->client_.disconnect();
        self->setState(MQTTConState::Disconnected);
        self->lastConAttempt_ = millis();
    }
    //Release the semaphore
    xSemaphoreGive(self->semaphore_);
    //Delete the task
    vTaskDelete( NULL );
}

void MQTT::connect()
{
    //Don't use MQTT if server is not filled
    if(Parameters::getInstance()->getMQTTServer().isEmpty()){
        return;
    }

    if(!xSemaphoreTake(semaphore_, 0)){
        return;
    }    

    // Loop until we're reconnected
    if (!client_.connected() && ((millis()-lastConAttempt_) > 5000)) {   
        setState(MQTTConState::Connecting);
        IPAddress mqttServerIP;
        int ret = WiFi.hostByName(Parameters::getInstance()->getMQTTServer().c_str(), mqttServerIP);
        if(ret != 1){
            Serial.print("Unable to resolve hostname: ");
            Serial.print(Parameters::getInstance()->getMQTTServer().c_str());
            Serial.println(" try again in 5 seconds");
            lastConAttempt_ = millis();
            goto release;
        }
        // Attempt to connect
        client_.setServer(mqttServerIP, Parameters::getInstance()->getMQTTPort());

        // Create a task to connect to MQTT server without 'blocking' the main loop
         xTaskCreate(
            connectTask,        /* Task function. */
            "MQTTConnect",      /* String with name of task. */
            4*1024,             /* Stack size in bytes. */
            this,               /* Parameter passed as input of the task */
            1,                  /* Priority of the task. */
            NULL);              /* Task handle. */
    }

release:
    xSemaphoreGive(semaphore_);
}

void MQTT::publishStatus()
{
    if(client_.connected()){
        String msg;
        StaticJsonDocument<210> root;
        root["profile"] = Parameters::getInstance()->getCurrentProfile();
        root["extTemp"] = Process::getInstance()->getExternalTemperature();
        root["floorTemp"] = Process::getInstance()->getFloorTemperature();
        root["relay"] = Process::getInstance()->getRelayState() ? "on" : "off";
        root["peakTime"] = Process::getInstance()->getPeakTimeState() ? "on" : "off";
        root["load"] = Process::getInstance()->getLoadPercent();
        serializeJson(root, msg);
        client_.publish((prefix_ + STATUS_PREFIX).c_str(), msg.c_str());
    }
}

void MQTT::loop()
{
    bool publishToMQTT = false;
    unsigned long now = millis();

    if (mqttState_ != MQTTConState::NotUsed){
        publishToMQTT = client_.loop();
        if(!publishToMQTT) {
            //Not connected of problem with updates
            connect();
        }
    }
    //Publish to MQTT if it's time to do
    if(publishToMQTT && ((now-lastPostTime_) > mqttPostingInterval)){
        publishStatus();
        lastPostTime_ = now;
    }
}

void MQTT::topicCallback(char* topic, byte* payload, unsigned int length)
{
    String data;
    data.reserve(length);
    for (unsigned int i = 0; i < length; i++) {
        data += (char)payload[i];
    }
    String strTopic(topic);
    if(strTopic == (getInstance()->prefix_ + PROFILE_PREFIX)){
        //New profile
        Parameters::getInstance()->setCurrentProfile(data);
    }/*else if(strTopic == (getInstance()->prefix_ + "/profile1Set")){
        
    }*/
    getInstance()->lastPostTime_ = 0;
}

void MQTT::reconfigure()
{
    if(!Parameters::getInstance()->getMQTTServer().isEmpty()){

        //Setup MQTT client callbacks and port
        client_.setServer(Parameters::getInstance()->getMQTTServer().c_str(),
                             Parameters::getInstance()->getMQTTPort());
        client_.setCallback(topicCallback);
        setState(MQTTConState::Connecting);
    }else{
        if(client_.connected()){
            client_.disconnect();
        }
        client_.setCallback(NULL);
        setState(MQTTConState::NotUsed);        
    }
    if(prefix_ != Parameters::getInstance()->getMQTTName()){
        if(client_.connected()){
            client_.disconnect();
        }
        prefix_ = Parameters::getInstance()->getMQTTName();
    }
}

void MQTT::refreshState()
{
    switch(mqttState_){
        case MQTTConState::Connected:
            lv_msg_send(EVT_NEW_MQTT_STATUS, "Connected");
            break;
        case MQTTConState::Connecting:
            lv_msg_send(EVT_NEW_MQTT_STATUS, "Connecting");
            break;
        case MQTTConState::Disconnected:
            lv_msg_send(EVT_NEW_MQTT_STATUS, "Disconnected");
            break;
        case MQTTConState::NotUsed:
            lv_msg_send(EVT_NEW_MQTT_STATUS, "Not used");
            break;
    }
}

void MQTT::setState(MQTTConState state){
    mqttState_ = state;
    refreshState();
}