#include "more.h"
#include "ui.h"
#include "ui_utils.h"
#include "events.h"
#ifndef SIMULATOR
#include "global_info.h"
#endif
lv_obj_t * ui_More;


void ui_event_more_loaded(lv_event_t * e)
{
#ifndef SIMULATOR
    refresh_network_info();
    refresh_io_info();
#endif    
}

void ui_event_btnBack(lv_event_t * e)
{
    ui_show_home();
}

void create_network_objects(lv_obj_t * tab)
{
    lv_obj_set_flex_flow(tab, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_all(tab, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_row(tab, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_column(tab, 0, LV_PART_MAIN);

    create_info_display<const char *>(tab, "WiFi SSID : ", "%s", EVT_NEW_WIFI_SSID);
    create_info_display<const char *>(tab, "WiFi mode : ", "%s", EVT_NEW_WIFI_MODE);
    create_info_display<int8_t>(tab, "WiFi RSSI : ", "%ddB", EVT_NEW_WIFI_RSSI);
    create_info_display<const char *>(tab, "IP address : ", "%s", EVT_NEW_IP_ADDRESS);
    create_info_display<const char *>(tab, "IP mask : ", "%s", EVT_NEW_IP_MASK);
    create_info_display<const char *>(tab, "IP gateway : ", "%s", EVT_NEW_IP_GATEWAY);    
    create_info_display<const char *>(tab, "DNS IP : ", "%s", EVT_NEW_IP_DNS);
}

void create_io_objects(lv_obj_t * tab)
{
    lv_obj_set_flex_flow(tab, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_all(tab, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_row(tab, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_column(tab, 0, LV_PART_MAIN);

    create_info_display<uint32_t>(tab, "Ext. temp. ADC : ", "%umV", EVT_NEW_EXT_TEMP_VOLT);
    create_info_display<double>(tab, "Ext. temp. ADC : ", "%.1fOhm", EVT_NEW_EXT_TEMP_RES);
    create_info_display<uint32_t>(tab, "Floor temp. ADC : ", "%umV", EVT_NEW_FLOOR_TEMP_VOLT);
    create_info_display<double>(tab, "Floor temp. ADC : ", "%.1fOhm", EVT_NEW_FLOOR_TEMP_RES);
    create_info_display<const char *>(tab, "Tariff input : ", "%s", EVT_NEW_TARIFF_STATE);
    create_info_display<const char *>(tab, "Relay output : ", "%s", EVT_NEW_RELAY_STATE);
}


void ui_More_screen_init(void)
{
    ui_More = lv_obj_create(NULL);
    lv_obj_clear_flag(ui_More, LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    lv_obj_t * ui_btnBack = lv_btn_create(ui_More);
    lv_obj_set_width(ui_btnBack, 80);
    lv_obj_set_height(ui_btnBack, 25);
    lv_obj_set_x(ui_btnBack, -5);
    lv_obj_set_y(ui_btnBack, -5);
    lv_obj_set_align(ui_btnBack, LV_ALIGN_BOTTOM_RIGHT);
    lv_obj_add_flag(ui_btnBack, LV_OBJ_FLAG_SCROLL_ON_FOCUS);     /// Flags
    lv_obj_clear_flag(ui_btnBack, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_add_event_cb(ui_btnBack, ui_event_btnBack, LV_EVENT_CLICKED, NULL);

    lv_obj_t * ui_lblBack = lv_label_create(ui_btnBack);
    lv_obj_set_width(ui_lblBack, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_lblBack, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_lblBack, LV_ALIGN_CENTER);
    lv_label_set_text(ui_lblBack, LV_SYMBOL_HOME " Back");

    lv_obj_t * tabview;
    tabview = lv_tabview_create(ui_More, LV_DIR_LEFT, 80);
    lv_obj_set_size(tabview, LV_PCT(100), 135);

    lv_obj_t * tab_btns = lv_tabview_get_tab_btns(tabview);
    lv_obj_set_style_radius(tab_btns, 5, LV_PART_MAIN);
    lv_obj_set_style_bg_color(tab_btns, lv_palette_darken(LV_PALETTE_GREY, 3), 0);

    /*Add 3 tabs (the tabs are page (lv_page) and can be scrolled*/
    lv_obj_t * tab1 = lv_tabview_add_tab(tabview, "Network");
    lv_obj_t * tab2 = lv_tabview_add_tab(tabview, "I/O");
    lv_obj_t * tab3 = lv_tabview_add_tab(tabview, "MQTT");

    /*Add content to the tabs*/
    create_network_objects(tab1);
    create_io_objects(tab2);


    lv_obj_t * label = lv_label_create(tab3);
    lv_label_set_text(label, "MQTT settings");

    lv_obj_add_event_cb(ui_More, ui_event_more_loaded, LV_EVENT_SCREEN_LOADED, NULL);
}