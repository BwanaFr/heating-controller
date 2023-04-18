/**
 *  Cumulative heater controler
 *  Alternative regulator for Delta Dore 125n
 *  https://github.com/BwanaFr/heating-controller
**/


#include <Arduino.h>

#include "screen.h"
#include "inout.h"
#include "lvgl.h"
#include "ui/ui.h"
#include "pin_config.h"
#include "process.h"

//Device parameters
#include "parameters.h"

//MQTT includes
#include <PubSubClient.h>
#include <ArduinoJson.h>

//ESPEasyCfg objects
AsyncWebServer server(80);
ESPEasyCfg captivePortal(&server, "Heater");
Parameters parameters;

//MQTT objects
WiFiClient espClient;                                   // TCP client
PubSubClient client(espClient);                         // MQTT object
const unsigned long mqttPostingInterval = 10L * 1000L;  // Delay between updates, in milliseconds
static unsigned long mqttLastPostTime = 0;              // Last time you sent to the server, in milliseconds
String mqttStatusService;                               // Status publishing service.
uint32_t lastMQTTConAttempt = 0;                        // Last MQTT connection attempt
enum class MQTTConState {Connecting, Connected, Disconnected, NotUsed};
MQTTConState mqttState = MQTTConState::Disconnected;

//Other variables
static bool systemReady = false;            // True when system is ready
Process process;                            //Class used to compute the heating value

/**
 * Callback to get captive-portal messages
*/
void captive_portal_message(const char* msg, ESPEasyCfgMessageType type) {
  Serial.println(msg);
}

/**
 * Reloads parameters
*/
void apply_parameters() {
  Parameters::getInstance()->refreshAndSave();  
}

/**
 * Callback of MQTT
 */
void mqtt_callback(char* topic, byte* payload, unsigned int length) {
  String data;
  data.reserve(length);
  for (unsigned int i = 0; i < length; i++) {
    data += (char)payload[i];
  }
  String strTopic(topic);
  bool saveSettings = false;
  /*if(strTopic == (mqttFanService + "/mode")){
  }*/
  mqttLastPostTime = 0;
}

/**
 * Performs setup of MQTT server
*/
void mqtt_setup(){
  if(!parameters.getMQTTServer().isEmpty()){
      //Build MQTT service names
    mqttStatusService =  parameters.getMQTTName() +  "/status";
    
    //Setup MQTT client callbacks and port
    client.setServer(parameters.getMQTTServer().c_str(), parameters.getMQTTPort());
    client.setCallback(mqtt_callback);
    mqttState = MQTTConState::Connecting;
  }else{
    mqttState = MQTTConState::NotUsed;
  }
}

/**
 * Callback from captive portal when state has changed
*/
void captive_portal_state(ESPEasyCfgState state) {
  if(state == ESPEasyCfgState::Reconfigured){
    apply_parameters();
    //Don't use MQTT if server is not filled
    if(parameters.getMQTTServer().isEmpty()){
      mqttState = MQTTConState::NotUsed;
    }else{
      mqtt_setup();
      mqttLastPostTime = 0;
      client.disconnect();
    }
  }
}

/**
 * Setups the captive portal
 * 
*/
void captive_portal_setup( void * parameter ) {
  //Configure captive portal
  parameters.init(&captivePortal);
  captivePortal.setLedPin(PIN_USER_LED);
  captivePortal.setMessageHandler(captive_portal_message);
  captivePortal.setStateHandler(captive_portal_state);
  //Start our captive portal (if not configured)
  //At first usage, you will find a new WiFi network named "Heater"
  captivePortal.begin();
  //Serve web pages
  server.begin();
  //System is ready!
  systemReady = true;
  //Terminates the FreeRTOS tasks
  vTaskDelete( NULL );
}

void mqtt_reconnect() {
  //Don't use MQTT if server is not filled
  if(parameters.getMQTTServer().isEmpty()){
    return;
  }
  // Loop until we're reconnected
  if (!client.connected() && ((millis()-lastMQTTConAttempt)>5000)) {   
    mqttState = MQTTConState::Connecting;
    IPAddress mqttServerIP;
    int ret = WiFi.hostByName(parameters.getMQTTServer().c_str(), mqttServerIP);
    if(ret != 1){
      Serial.print("Unable to resolve hostname: ");
      Serial.print(parameters.getMQTTServer().c_str());
      Serial.println(" try again in 5 seconds");
      lastMQTTConAttempt = millis();
      return;
    }
    Serial.print("Attempting MQTT connection to ");
    Serial.print(parameters.getMQTTServer().c_str());
    Serial.print(':');
    Serial.print(parameters.getMQTTPort());
    Serial.print('(');
    Serial.print(mqttServerIP);
    Serial.print(")...");
    // Create a Client ID baased on MAC address
    byte mac[6];                     // the MAC address of your Wifi shield
    WiFi.macAddress(mac);
    String clientId = "Heater-";
    clientId += String(mac[3], HEX);
    clientId += String(mac[4], HEX);
    clientId += String(mac[5], HEX);
    // Attempt to connect
    client.setServer(mqttServerIP, parameters.getMQTTPort());
    if((ret == 1) && (client.connect(clientId.c_str(), parameters.getMQTTUser().c_str(), parameters.getMQTTPass().c_str()))) {
      Serial.println("connected");
      mqttState = MQTTConState::Connected;
      //Subscribe to MQTT topics
      /*client.subscribe((mqttFanService + "/mode").c_str());
      client.subscribe((mqttFanService + "/percentage").c_str());
      client.subscribe((mqttFanService + "/set").c_str());*/
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      client.disconnect();
      mqttState = MQTTConState::Disconnected;
      lastMQTTConAttempt = millis();
    }
  }
}

/**
 * This method publishes acquisition values over MQTT
 * Values are :
 * - Fan speed
 * - Temperature 1 sensor value
 * - Temperature 2 sensor value
 * - System operating mode (AUTO, FORCED, DISABLED)
*/
void publishValuesToMQTT(){
  //Publish to MQTT clients
  if(client.connected()){
    /*String msg;
    StaticJsonDocument<210> root;
    root["percentage"] = fanPercent.getValue();
    root["temperature1"] = temperature1;
    root["temperature2"] = temperature2;
    root["offTemperature"] = offTemperature.getValue();
    root["onTemperature"] = onTemperature.getValue();
    root["mode"] = operMode.toString();
    root["state"] = fansOn ? "on" : "off";
    serializeJson(root, msg);
    client.publish(mqttStatusService.c_str(), msg.c_str());*/
  }
}

/**
 * Arduino Setup method
*/
void setup() {
  Serial.begin(115200);
  Serial.println("Heating-controller alive!");
  //Setup the LCD screen  
  setup_screen();
  //Setup I/O
  setup_inputs_outputs();
  
  //Create a task to initialize the Wifi (take long time, so we keep LVGL alive)
  xTaskCreate(
            captive_portal_setup,          /* Task function. */
            "WifiSetup",        /* String with name of task. */
            8*1024,            /* Stack size in bytes. */
            NULL,             /* Parameter passed as input of the task */
            1,                /* Priority of the task. */
            NULL);            /* Task handle. */
}

static unsigned long lastCheck = 0;
static int temp = 0;
static bool prevSystemReady = false;

void loop() {
  unsigned long now = millis();
  bool publishToMQTT = false;
  
  //MQTT loop
  if (mqttState != MQTTConState::NotUsed){
    publishToMQTT = client.loop();
    if(!publishToMQTT) {
      //Not connected of problem with updates
      mqtt_reconnect();
    }
  }

  //LVGL and screen management loop
  loop_screen(systemReady);

  //If captive portal is ready, load parameters
  if(systemReady && !prevSystemReady){
    apply_parameters();
    //Initialize MQTT
    mqtt_setup();
  }

  if(systemReady){
    //Do it every second, it's enough
    if((now-lastCheck) > 1000){
      lastCheck = now;
      double extTemp = getExternalTemperature();
      double floorTemp = getFloorTemperature();
      bool peakTime = getTariffInput();
      bool relay = process.setIOState(extTemp, floorTemp, peakTime);
      setRelay(relay);
    }

    //Publish to MQTT if it's time to do
    if(publishToMQTT && ((now-mqttLastPostTime)>mqttPostingInterval)){
      publishValuesToMQTT();
      mqttLastPostTime = now;
    }
    parameters.loop();
  }

  prevSystemReady = systemReady;
  yield();
}