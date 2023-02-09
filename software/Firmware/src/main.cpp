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
#include "settings.h"

//ESPEasyCfg includes
#include <ESPEasyCfg.h>

//ESPEasyCfg objects
AsyncWebServer server(80);
ESPEasyCfg captivePortal(&server, "Heater");
ESPEasyCfgParameterGroup mqttParamGrp("MQTT");
ESPEasyCfgParameter<String> mqttServer(mqttParamGrp, "mqttServer", "MQTT server", "server.local");
ESPEasyCfgParameter<String> mqttUser(mqttParamGrp, "mqttUser", "MQTT user", "user");
ESPEasyCfgParameter<String> mqttPass(mqttParamGrp, "mqttPass", "MQTT password", "");
ESPEasyCfgParameter<int> mqttPort(mqttParamGrp, "mqttPort", "MQTT port", 1883);

//Process objects
ProcessSettings processConfig;

/**
 * Callback to get captive-portal messages
*/
void captive_portal_message(const char* msg, ESPEasyCfgMessageType type) {
  Serial.println(msg);
}

void captive_portal_setup( void * parameter ) {
 //Configure captive portal
  //The MQTT password is a password HTML type
  mqttPass.setInputType("password");
  //Finally, add our parameter group to the captive portal
  captivePortal.addParameterGroup(&mqttParamGrp);
  captivePortal.setLedPin(PIN_USER_LED);
  captivePortal.setMessageHandler(captive_portal_message);
  //Start our captive portal (if not configured)
  //At first usage, you will find a new WiFi network named "Heater"
  captivePortal.begin();
  //Serve web pages
  server.begin();
  set_system_ready();
  vTaskDelete( NULL );
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
  //Initialize process settings
  processConfig.loadFromFile();

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
void loop() {
  unsigned long now = millis();
  loop_screen();
  if((now-lastCheck) > 1000){
    lastCheck = now;
    unsigned long startADC = micros();
    double extTemp = getExternalTemperature();
    unsigned long elapsed = micros() - startADC;
    Serial.printf("ADC conversion took : %uus\n", elapsed);
    double floorTemp = getFloorTemperature();
  }
  delay(2);  
}