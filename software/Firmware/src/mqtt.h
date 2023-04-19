#ifndef _MQTT_H_
#define _MQTT_H_

#include <Arduino.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <WiFi.h>

/**
 * This class manages all MQTT communication
*/
class MQTT {
public:
    enum class MQTTConState {Connecting, Connected, Disconnected, NotUsed};

    MQTT();
    virtual ~MQTT() = default;
    void loop();
    void reconfigure();
    void refreshState();

    static void topicCallback(char* topic, byte* payload, unsigned int length);
    inline static MQTT* getInstance(){ return instance_; }
private:
    WiFiClient espClient_;                                              // TCP client
    PubSubClient client_;                                               // MQTT object    
    unsigned long lastPostTime_ ;                                       // Last time you sent to the server, in milliseconds    
    uint32_t lastConAttempt_;                                           // Last MQTT connection attempt
    MQTTConState mqttState_;                                            // State of the mqtt connection
    String prefix_;                                                     // Prefix name

    void connect();
    void publishStatus();                                               //Publish status to MQTT
    void setState(MQTTConState state);

    static MQTT* instance_;                                             // Singleton
    static constexpr unsigned long mqttPostingInterval = 10L * 1000L;   // Delay between updates, in milliseconds
};


#endif