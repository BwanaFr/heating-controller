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

// Objects for heating process configuration
ESPEasyCfgParameterGroup heatingParamGrp("Heating");
ESPEasyCfgEnumParameter currentProfile(heatingParamGrp, "heatProfile", "Heating profile", "Profile 1;Profile 2", "Profile 1");
ESPEasyCfgParameter<int> timeBase(heatingParamGrp, "heatTimeBase", "Heating time-base [s]", 60);
ESPEasyCfgParameter<double> tempLimit(heatingParamGrp, "tempLimit", "Floor temperature limit [C]", 35);

ESPEasyCfgParameterGroup profile1ParamGrp("Profile 1");
ESPEasyCfgParameterGroup profile1HParamGrp("Peak time");
ESPEasyCfgParameter<double> profile1HOffTemp(profile1HParamGrp, "profile1HOffTemp", "Off temperature", 5.0);
ESPEasyCfgParameter<double> profile1HFullTemp(profile1HParamGrp, "profile1HFullTemp", "Full temperature", 0.0);
ESPEasyCfgParameterGroup profile1LParamGrp("Off-peak time");
ESPEasyCfgParameter<double> profile1LOffTemp(profile1LParamGrp, "profile1LOffTemp", "Off temperature", 12.0);
ESPEasyCfgParameter<double> profile1LFullTemp(profile1LParamGrp, "profile1LFullTemp", "Full temperature", 5.0);

ESPEasyCfgParameterGroup profile2ParamGrp("Profile 2");
ESPEasyCfgParameterGroup profile2HParamGrp("Peak time");
ESPEasyCfgParameter<double> profile2HOffTemp(profile2HParamGrp, "profile2HOffTemp", "Off temperature", 5.0);
ESPEasyCfgParameter<double> profile2HFullTemp(profile2HParamGrp, "profile2HFullTemp", "Full temperature", 0.0);
ESPEasyCfgParameterGroup profile2LParamGrp("Off-peak time");
ESPEasyCfgParameter<double> profile2LOffTemp(profile2LParamGrp, "profile2LOffTemp", "Off temperature", 12.0);
ESPEasyCfgParameter<double> profile2LFullTemp(profile2LParamGrp, "profile2LFullTemp", "Full temperature", 5.0);

// System objects for ADC
ESPEasyCfgParameterGroup systemParamGrp("System");
ESPEasyCfgParameter<double> adcRefVoltage(systemParamGrp, "adcRefVoltage", "ADC reference voltage [mV]", 3112.0);
ESPEasyCfgParameter<int> adcAveraging(systemParamGrp, "adcAveraging", "ADC averaging [samples]", 100, "", "{\"min\":\"0\", \"max\":\"500\"}");


static bool systemReady = false;            // True when system is ready
Process process;

/**
 * Setup captive portal parameters for heating
 * TODO: Update ESPEasyCfg for adding it in the constructor
*/
void heater_parameters_setup() {
    // Adds profile 1 parameters
    profile1ParamGrp.add(&profile1HParamGrp);
    profile1ParamGrp.add(&profile1LParamGrp);
    heatingParamGrp.add(&profile1ParamGrp);
    // Adds provile 2 parameters
    profile2ParamGrp.add(&profile2HParamGrp);
    profile2ParamGrp.add(&profile2LParamGrp);
    heatingParamGrp.add(&profile2ParamGrp);
    // Adds heating to parameters
    captivePortal.addParameterGroup(&heatingParamGrp);
}

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
  Serial.println("Applying parameters!");
  //Sets ADC parameter
  setADCVRef(adcRefVoltage.getValue());
  setNbAverages(adcAveraging.getValue());
  //Sets process parameters
  if(currentProfile.toString() == "Profile 1") {
    process.setOffPeakTimeParameters(profile1LOffTemp.getValue(), profile1LFullTemp.getValue());
    process.setPeakTimeParameters(profile1HOffTemp.getValue(), profile1HFullTemp.getValue());
  }else{
    process.setOffPeakTimeParameters(profile2LOffTemp.getValue(), profile2LFullTemp.getValue());
    process.setPeakTimeParameters(profile2HOffTemp.getValue(), profile2HFullTemp.getValue());
  }
  process.setTemperatureLimit(tempLimit.getValue());
  process.setTimebase(timeBase.getValue());
  lv_msg_send(EVT_NEW_HEATING_PROFILE, currentProfile.toString().c_str());
}

/**
 * Callback from captive portal when state has changed
*/
void captive_portal_state(ESPEasyCfgState state) {
  if(state == ESPEasyCfgState::Reconfigured){
    Serial.println("CP reconfigured!");
    apply_parameters();
    //TODO: Updates settings
    /*changeMode(operMode.toString());
    //Don't use MQTT if server is not filled
    if(mqttServer.getValue().isEmpty()){
      mqttState = MQTTConState::NotUsed;
    }else{
      mqtt_setup();
      mqttLastPostTime = 0;
      client.disconnect();
    }*/
  }
}

/**
 * Setups the captive portal
 * 
*/
void captive_portal_setup( void * parameter ) {
 //Configure captive portal
  //The MQTT password is a password HTML type
  mqttPass.setInputType("password");
  //Finally, add our parameter group to the captive portal
  captivePortal.addParameterGroup(&mqttParamGrp);
  // Adds parameters for heating process
  heater_parameters_setup();
  // Adds parameters for system
  captivePortal.addParameterGroup(&systemParamGrp);
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
  loop_screen(systemReady);
  if(systemReady && !prevSystemReady){
    apply_parameters();
  }

  if(systemReady){
    if((now-lastCheck) > 1000){
      lastCheck = now;
      double extTemp = getExternalTemperature();
      double floorTemp = getFloorTemperature();
      bool peakTime = getTariffInput();
      bool relay = process.setIOState(extTemp, floorTemp, peakTime);
      setRelay(relay);
    }
  }

  prevSystemReady = systemReady;
  yield();
}