#include "global_info.h"
#include "ui/ui.h"
#include "WiFi.h"
#include "esp_wifi.h"

#include "inout.h"

void refresh_network_info(void)
{
  // Gets WiFi informations
  wifi_mode_t mode;
  esp_err_t err = esp_wifi_get_mode(&mode);
  if(err != ESP_OK){
    if(err == ESP_ERR_WIFI_NOT_INIT){
      lv_msg_send(EVT_NEW_WIFI_SSID, "Not initialized");
    }else{
      lv_msg_send(EVT_NEW_WIFI_SSID, "Unknown error");
    }
    lv_msg_send(EVT_NEW_WIFI_RSSI, nullptr);
    lv_msg_send(EVT_NEW_WIFI_MODE, nullptr);
    lv_msg_send(EVT_NEW_IP_ADDRESS, nullptr);
    lv_msg_send(EVT_NEW_IP_MASK, nullptr);
    lv_msg_send(EVT_NEW_IP_GATEWAY, nullptr);
    lv_msg_send(EVT_NEW_IP_DNS, nullptr);
  }else if(mode == WIFI_MODE_NULL){
    lv_msg_send(EVT_NEW_WIFI_SSID, "-------");    
  }else if(mode == WIFI_MODE_AP){
    wifi_config_t conf;
    esp_wifi_get_config((wifi_interface_t)WIFI_IF_AP, &conf);
    lv_msg_send(EVT_NEW_WIFI_SSID, reinterpret_cast<const char*>(conf.ap.ssid));
    lv_msg_send(EVT_NEW_WIFI_RSSI, nullptr);
    lv_msg_send(EVT_NEW_WIFI_MODE, "AP");
    lv_msg_send(EVT_NEW_IP_ADDRESS, WiFi.softAPIP().toString().c_str());
    lv_msg_send(EVT_NEW_IP_MASK, nullptr);
    lv_msg_send(EVT_NEW_IP_GATEWAY, nullptr);
    lv_msg_send(EVT_NEW_IP_DNS, nullptr);
  }else{
    wifi_config_t conf;
    esp_wifi_get_config((wifi_interface_t)WIFI_IF_STA, &conf);
    lv_msg_send(EVT_NEW_WIFI_SSID, reinterpret_cast<const char*>(conf.sta.ssid));
    wifi_ap_record_t ap_info;
    esp_wifi_sta_get_ap_info(&ap_info);
    lv_msg_send(EVT_NEW_WIFI_RSSI, &ap_info.rssi);
    lv_msg_send(EVT_NEW_WIFI_MODE, "Client");
    lv_msg_send(EVT_NEW_IP_ADDRESS, WiFi.localIP().toString().c_str());
    lv_msg_send(EVT_NEW_IP_MASK, WiFi.subnetMask().toString().c_str());
    lv_msg_send(EVT_NEW_IP_GATEWAY, WiFi.gatewayIP().toString().c_str());
    lv_msg_send(EVT_NEW_IP_DNS, WiFi.dnsIP().toString().c_str());
  }
}

void refresh_io_info(void)
{
  //Read ADC, it will propagate proper events
  getExternalTemperature();
  getFloorTemperature();
  bool relay = false;
  lv_msg_send(EVT_NEW_RELAY_STATE, relay ? "ON" : "OFF");
  bool tariff = false;
  lv_msg_send(EVT_NEW_TARIFF_STATE, tariff ? "Off-peak" : "Peak");
}