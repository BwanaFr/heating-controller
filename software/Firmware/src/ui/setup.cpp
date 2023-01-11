#include "setup.h"
#include "ui.h"

lv_obj_t * ui_Setup;
lv_obj_t * ui_btnBackMain;
lv_obj_t * ui_lblBackMain;
lv_obj_t * ui_setupPanel;
lv_obj_t * ui_netPanel;
lv_obj_t * ui_btnNetwork;
lv_obj_t * ui_netLabel;
lv_obj_t * ui_heatPanel;
lv_obj_t * ui_btnHeater;
lv_obj_t * ui_heatLabel;

void ui_event_btnBackMain(lv_event_t * e)
{
    // lv_event_code_t event_code = lv_event_get_code(e);
    // lv_obj_t * target = lv_event_get_target(e);
    // if(event_code == LV_EVENT_CLICKED) {
        ui_show_home();
    // }
}

void ui_Setup_screen_init(void)
{
    ui_Setup = lv_obj_create(NULL);
    lv_obj_clear_flag(ui_Setup, LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    ui_btnBackMain = lv_btn_create(ui_Setup);
    lv_obj_set_width(ui_btnBackMain, 80);
    lv_obj_set_height(ui_btnBackMain, 25);
    lv_obj_set_x(ui_btnBackMain, -5);
    lv_obj_set_y(ui_btnBackMain, -5);
    lv_obj_set_align(ui_btnBackMain, LV_ALIGN_BOTTOM_RIGHT);
    lv_obj_add_flag(ui_btnBackMain, LV_OBJ_FLAG_SCROLL_ON_FOCUS);     /// Flags
    lv_obj_clear_flag(ui_btnBackMain, LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    ui_lblBackMain = lv_label_create(ui_btnBackMain);
    lv_obj_set_width(ui_lblBackMain, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_lblBackMain, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_lblBackMain, LV_ALIGN_CENTER);
    lv_label_set_text(ui_lblBackMain, LV_SYMBOL_HOME " Back");

    ui_setupPanel = lv_obj_create(ui_Setup);
    lv_obj_set_height(ui_setupPanel, 130);
    lv_obj_set_width(ui_setupPanel, lv_pct(100));
    lv_obj_set_align(ui_setupPanel, LV_ALIGN_TOP_MID);

    ui_netPanel = lv_obj_create(ui_setupPanel);
    lv_obj_set_width(ui_netPanel, 100);
    lv_obj_set_height(ui_netPanel, 84);
    lv_obj_set_align(ui_netPanel, LV_ALIGN_LEFT_MID);
    lv_obj_clear_flag(ui_netPanel, LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    ui_btnNetwork = lv_imgbtn_create(ui_netPanel);
    lv_imgbtn_set_src(ui_btnNetwork, LV_IMGBTN_STATE_RELEASED, NULL, &ui_img_network_png, NULL);
    lv_obj_set_width(ui_btnNetwork, 40);
    lv_obj_set_height(ui_btnNetwork, 32);
    lv_obj_set_align(ui_btnNetwork, LV_ALIGN_TOP_MID);

    ui_netLabel = lv_label_create(ui_netPanel);
    lv_obj_set_width(ui_netLabel, lv_pct(100));
    lv_obj_set_height(ui_netLabel, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_netLabel, LV_ALIGN_BOTTOM_MID);
    lv_label_set_text(ui_netLabel, "Network");
    lv_obj_set_style_text_align(ui_netLabel, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_heatPanel = lv_obj_create(ui_setupPanel);
    lv_obj_set_width(ui_heatPanel, 100);
    lv_obj_set_height(ui_heatPanel, 84);
    lv_obj_set_align(ui_heatPanel, LV_ALIGN_RIGHT_MID);
    lv_obj_clear_flag(ui_heatPanel, LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    ui_btnHeater = lv_imgbtn_create(ui_heatPanel);
    lv_imgbtn_set_src(ui_btnHeater, LV_IMGBTN_STATE_RELEASED, NULL, &ui_img_heat_pump_png, NULL);
    lv_obj_set_width(ui_btnHeater, 40);
    lv_obj_set_height(ui_btnHeater, 32);
    lv_obj_set_align(ui_btnHeater, LV_ALIGN_TOP_MID);

    ui_heatLabel = lv_label_create(ui_heatPanel);
    lv_obj_set_width(ui_heatLabel, lv_pct(100));
    lv_obj_set_height(ui_heatLabel, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_heatLabel, LV_ALIGN_BOTTOM_MID);
    lv_label_set_text(ui_heatLabel, "Heating");
    lv_obj_set_style_text_align(ui_heatLabel, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_add_event_cb(ui_btnBackMain, ui_event_btnBackMain, LV_EVENT_CLICKED, NULL);

}