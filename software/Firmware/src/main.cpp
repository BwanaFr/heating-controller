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
#include "parameters.h"
#include "mqtt.h"

//For OTA update
#include <Update.h>

//MQTT includes
#include <PubSubClient.h>
#include <ArduinoJson.h>

//ESPEasyCfg objects
AsyncWebServer server(80);
ESPEasyCfg captivePortal(&server, "Heater");
Parameters parameters;

//MQTT objects
MQTT mqtt;

//Other variables
static bool systemReady = false;            // True when system is ready
Process process;                            // Class used to compute the heating value
size_t otaContentLenght;                    // Size of the OTA update file
/**
 * Callback to get captive-portal messages
*/
void captive_portal_message(const char* msg, ESPEasyCfgMessageType type) {
  Serial.println(msg);
}

/**
 * Callback from captive portal when state has changed
*/
void captive_portal_state(ESPEasyCfgState state) {
  if(state == ESPEasyCfgState::Reconfigured){
    Parameters::getInstance()->captivePortalReconfigured();  
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

/**
 * Setups OTA update
*/
void setup_ota_update() {
  server.on("/www/update", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/html", "<html><body><form method='POST' action='/www/update' enctype='multipart/form-data'><input type='file' name='update'><input type='submit' value='Update'></form></body></html>");
  });
  server.on("/www/update", HTTP_POST,
    [](AsyncWebServerRequest *request) {},
    [](AsyncWebServerRequest *request, const String& filename, size_t index, uint8_t *data, size_t len, bool final) {
                    if (!index){
                      Serial.println("Update");
                      otaContentLenght = request->contentLength();
                      // if filename includes spiffs, update the spiffs partition
                      int cmd = (filename.indexOf("spiffs") > -1) ? U_SPIFFS : U_FLASH;
                      if (!Update.begin(otaContentLenght, cmd)) {
                        Update.printError(Serial);
                      }
                    }

                    if (Update.write(data, len) != len) {
                      Update.printError(Serial);
                    } else {
                      Serial.printf("Progress: %d%%\n", (Update.progress()*100)/Update.size());
                    }

                    if (final) {
                      AsyncWebServerResponse *response = request->beginResponse(302, "text/plain", "Please wait while the device reboots");
                      response->addHeader("Refresh", "20");  
                      response->addHeader("Location", "/");
                      request->send(response);
                      if (!Update.end(true)){
                        Update.printError(Serial);
                      } else {
                        Serial.println("Update complete");
                        Serial.flush();
                        ESP.restart();
                      }
                    }
                  }
  );
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
  //Setup OTA
  setup_ota_update();

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
  
  //LVGL and screen management loop
  loop_screen(systemReady);

  //If captive portal is ready, load parameters
  if(systemReady && !prevSystemReady){
    Parameters::getInstance()->captivePortalReconfigured();
  }

  if(systemReady){
    //Do it every second, it's enough
    if((now-lastCheck) >= 1000){
      lastCheck = now;
      double extTemp = getExternalTemperature();
      double floorTemp = getFloorTemperature();
      bool peakTime = getTariffInput();
      bool relay = process.setIOState(extTemp, floorTemp, peakTime);
      setRelay(relay);
    }
    mqtt.loop();
    
    parameters.loop();
  }

  prevSystemReady = systemReady;
  delayMicroseconds(100);
}