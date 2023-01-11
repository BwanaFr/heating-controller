/**
 *  Cumulative heater controler
 *  Alternative regulator for Delta Dore 125n
 *  https://github.com/BwanaFr/heating-controller
**/


#include <Arduino.h>

#include "screen.h"
#include "lvgl.h"
#include "ui/ui.h"

//ESPEasyCfg includes
#include <ESPEasyCfg.h>

//ESPEasyCfg objects
AsyncWebServer server(80);
ESPEasyCfg captivePortal(&server, "Heater");
ESPEasyCfgParameterGroup mqttParamGrp("MQTT");
ESPEasyCfgParameter<String> mqttServer("mqttServer", "MQTT server", "server.local");
ESPEasyCfgParameter<String> mqttUser("mqttUser", "MQTT user", "user");
ESPEasyCfgParameter<String> mqttPass("mqttPass", "MQTT password", "");
ESPEasyCfgParameter<int> mqttPort("mqttPort", "MQTT port", 1883);


/**
 * Arduino Setup method
*/
void setup() {
  Serial.begin(115200);
  Serial.println("Heating-controller alive!");

  setup_screen();

  //Configure captive portal
  //The MQTT password is a password HTML type
  mqttPass.setInputType("password");
  //Add created parameters to the MQTT parameter group
  mqttParamGrp.add(&mqttServer);
  mqttParamGrp.add(&mqttUser);
  mqttParamGrp.add(&mqttPass);
  mqttParamGrp.add(&mqttPort);
  //Finally, add our parameter group to the captive portal
  captivePortal.addParameterGroup(&mqttParamGrp);
  
  //Start our captive portal (if not configured)
  //At first usage, you will find a new WiFi network named "MyThing"
  captivePortal.begin();
  //Serve web pages
  server.begin();
}



static unsigned long lastCheck = 0;
static int temp = 0;
static double percent = 0.0;
void loop() {

  unsigned long now = millis();
  loop_screen();
  if((now-lastCheck) > 1000){
    percent = ((++temp%1000)/10.0);
    lastCheck = now;
    lv_msg_send(EVT_NEW_EXT_TEMP, &percent);
  }
  delay(2);
}