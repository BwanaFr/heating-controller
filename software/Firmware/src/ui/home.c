#include "home.h"

#include "events.h"

///////////////////// VARIABLES ////////////////////
lv_obj_t * ui_Home;
void ui_event_btnSetup(lv_event_t * e);
lv_obj_t * ui_btnSetup;
lv_obj_t * ui_lblSetup;
lv_obj_t * ui_panTempExt;
lv_obj_t * ui_lblTExt;
lv_obj_t * ui_lblTExtTitle;
lv_obj_t * ui_panTempLim;
lv_obj_t * ui_lblTLim;
lv_obj_t * ui_lblTLimTitle;
lv_obj_t * ui_panSetpoint;
lv_obj_t * ui_lblSetpoint;
lv_obj_t * ui_lblSetpointTitle;
lv_obj_t * ui_panOpMode;
lv_obj_t * ui_lblOpModeTitle;
lv_obj_t * ui_lblOpModeProfileTitle;
lv_obj_t * ui_lblOpModeTZero;
lv_obj_t * ui_lblOpModeTHundred;
lv_obj_t * ui_lblOpModeTimebase;
void ui_event_btnMOre(lv_event_t * e);
lv_obj_t * ui_btnMOre;
lv_obj_t * ui_lblMore;

void ui_event_btnSetup(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_CLICKED) {
        btnSetupClicked(e);
    }
}
void ui_event_btnMOre(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_CLICKED) {
        btnMoreClicked(e);
    }
}

void update_double_cb(lv_event_t *e){
    lv_obj_t *label = lv_event_get_target(e);
    lv_msg_t *m = lv_event_get_msg(e);

    const char *fmt = (const char *)lv_msg_get_user_data(m);
    const double *v = (const double *)lv_msg_get_payload(m);
    lv_label_set_text_fmt(label, fmt, *v);
}

void update_int32_cb(lv_event_t *e){
    lv_obj_t *label = lv_event_get_target(e);
    lv_msg_t *m = lv_event_get_msg(e);

    const char *fmt = (const char *)lv_msg_get_user_data(m);
    const int32_t *v = (const int32_t *)lv_msg_get_payload(m);
    lv_label_set_text_fmt(label, fmt, *v);
}

void update_str_cb(lv_event_t *e){
    lv_obj_t *label = lv_event_get_target(e);
    lv_msg_t *m = lv_event_get_msg(e);

    const char *fmt = (const char *)lv_msg_get_user_data(m);
    const char *v = (const char *)lv_msg_get_payload(m);
    lv_label_set_text_fmt(label, fmt, *v);
}

void ui_Home_screen_init(void)
{
    ui_Home = lv_obj_create(NULL);
    lv_obj_clear_flag(ui_Home, LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    ui_btnSetup = lv_btn_create(ui_Home);
    lv_obj_set_width(ui_btnSetup, 80);
    lv_obj_set_height(ui_btnSetup, 30);
    lv_obj_set_x(ui_btnSetup, -12);
    lv_obj_set_y(ui_btnSetup, -10);
    lv_obj_set_align(ui_btnSetup, LV_ALIGN_BOTTOM_RIGHT);
    lv_obj_add_flag(ui_btnSetup, LV_OBJ_FLAG_SCROLL_ON_FOCUS);     /// Flags
    lv_obj_clear_flag(ui_btnSetup, LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    ui_lblSetup = lv_label_create(ui_btnSetup);
    lv_obj_set_width(ui_lblSetup, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_lblSetup, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_lblSetup, 0);
    lv_obj_set_y(ui_lblSetup, 1);
    lv_obj_set_align(ui_lblSetup, LV_ALIGN_CENTER);
    lv_label_set_text(ui_lblSetup, "SETUP");

    ui_panTempExt = lv_obj_create(ui_Home);
    lv_obj_set_width(ui_panTempExt, 95);
    lv_obj_set_height(ui_panTempExt, 75);
    lv_obj_set_x(ui_panTempExt, 5);
    lv_obj_set_y(ui_panTempExt, 5);
    lv_obj_clear_flag(ui_panTempExt, LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    ui_lblTExt = lv_label_create(ui_panTempExt);
    lv_obj_set_width(ui_lblTExt, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_lblTExt, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_lblTExt, 0);
    lv_obj_set_y(ui_lblTExt, 15);
    lv_obj_set_align(ui_lblTExt, LV_ALIGN_BOTTOM_MID);
    lv_label_set_text(ui_lblTExt, "---");
    lv_obj_set_style_text_align(ui_lblTExt, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_lblTExt, &lv_font_montserrat_42, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_event_cb(ui_lblTExt, update_double_cb, LV_EVENT_MSG_RECEIVED, NULL);
    lv_msg_subsribe_obj(EVT_NEW_EXT_TEMP, ui_lblTExt, (void *)"%.1lf");

    ui_lblTExtTitle = lv_label_create(ui_panTempExt);
    lv_obj_set_width(ui_lblTExtTitle, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_lblTExtTitle, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_lblTExtTitle, 0);
    lv_obj_set_y(ui_lblTExtTitle, -5);
    lv_obj_set_align(ui_lblTExtTitle, LV_ALIGN_TOP_MID);
    lv_label_set_text(ui_lblTExtTitle, "T. out [°C]");

    ui_panTempLim = lv_obj_create(ui_Home);
    lv_obj_set_width(ui_panTempLim, 95);
    lv_obj_set_height(ui_panTempLim, 75);
    lv_obj_set_x(ui_panTempLim, -5);
    lv_obj_set_y(ui_panTempLim, 5);
    lv_obj_set_align(ui_panTempLim, LV_ALIGN_TOP_RIGHT);
    lv_obj_clear_flag(ui_panTempLim, LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    ui_lblTLim = lv_label_create(ui_panTempLim);
    lv_obj_set_width(ui_lblTLim, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_lblTLim, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_lblTLim, 0);
    lv_obj_set_y(ui_lblTLim, 15);
    lv_obj_set_align(ui_lblTLim, LV_ALIGN_BOTTOM_MID);
    lv_label_set_text(ui_lblTLim, "---");
    lv_obj_set_style_text_align(ui_lblTLim, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_lblTLim, &lv_font_montserrat_42, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_event_cb(ui_lblTLim, update_double_cb, LV_EVENT_MSG_RECEIVED, NULL);
    lv_msg_subsribe_obj(EVT_NEW_FLOOR_TEMP, ui_lblTLim, (void *)"%.1f");

    ui_lblTLimTitle = lv_label_create(ui_panTempLim);
    lv_obj_set_width(ui_lblTLimTitle, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_lblTLimTitle, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_lblTLimTitle, 0);
    lv_obj_set_y(ui_lblTLimTitle, -5);
    lv_obj_set_align(ui_lblTLimTitle, LV_ALIGN_TOP_MID);
    lv_label_set_text(ui_lblTLimTitle, "T. floor [°C]");

    ui_panSetpoint = lv_obj_create(ui_Home);
    lv_obj_set_width(ui_panSetpoint, 105);
    lv_obj_set_height(ui_panSetpoint, 75);
    lv_obj_set_x(ui_panSetpoint, 0);
    lv_obj_set_y(ui_panSetpoint, 5);
    lv_obj_set_align(ui_panSetpoint, LV_ALIGN_TOP_MID);
    lv_obj_clear_flag(ui_panSetpoint, LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    ui_lblSetpoint = lv_label_create(ui_panSetpoint);
    lv_obj_set_width(ui_lblSetpoint, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_lblSetpoint, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_lblSetpoint, 0);
    lv_obj_set_y(ui_lblSetpoint, 15);
    lv_obj_set_align(ui_lblSetpoint, LV_ALIGN_BOTTOM_MID);
    lv_label_set_text(ui_lblSetpoint, "---%");
    lv_obj_set_style_text_align(ui_lblSetpoint, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_lblSetpoint, &lv_font_montserrat_42, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_event_cb(ui_lblTLim, update_int32_cb, LV_EVENT_MSG_RECEIVED, NULL);
    lv_msg_subsribe_obj(EVT_NEW_SET_POINT, ui_lblTLim, (void *)"%02d%");

    ui_lblSetpointTitle = lv_label_create(ui_panSetpoint);
    lv_obj_set_width(ui_lblSetpointTitle, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_lblSetpointTitle, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_lblSetpointTitle, 0);
    lv_obj_set_y(ui_lblSetpointTitle, -5);
    lv_obj_set_align(ui_lblSetpointTitle, LV_ALIGN_TOP_MID);
    lv_label_set_text(ui_lblSetpointTitle, "Setpoint");

    ui_panOpMode = lv_obj_create(ui_Home);
    lv_obj_set_width(ui_panOpMode, 209);
    lv_obj_set_height(ui_panOpMode, 75);
    lv_obj_set_x(ui_panOpMode, 5);
    lv_obj_set_y(ui_panOpMode, -5);
    lv_obj_set_align(ui_panOpMode, LV_ALIGN_BOTTOM_LEFT);
    lv_obj_clear_flag(ui_panOpMode, LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    ui_lblOpModeTitle = lv_label_create(ui_panOpMode);
    lv_obj_set_width(ui_lblOpModeTitle, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_lblOpModeTitle, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_lblOpModeTitle, 0);
    lv_obj_set_y(ui_lblOpModeTitle, -14);
    lv_obj_set_align(ui_lblOpModeTitle, LV_ALIGN_TOP_MID);
    lv_label_set_text(ui_lblOpModeTitle, "Operating mode");

    ui_lblOpModeProfileTitle = lv_label_create(ui_panOpMode);
    lv_obj_set_width(ui_lblOpModeProfileTitle, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_lblOpModeProfileTitle, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_y(ui_lblOpModeProfileTitle, 2);
    lv_obj_set_x(ui_lblOpModeProfileTitle, lv_pct(-7));
    lv_label_set_text(ui_lblOpModeProfileTitle, "Current profile:");

    ui_lblOpModeTZero = lv_label_create(ui_panOpMode);
    lv_obj_set_width(ui_lblOpModeTZero, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_lblOpModeTZero, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_y(ui_lblOpModeTZero, 15);
    lv_obj_set_x(ui_lblOpModeTZero, lv_pct(-7));
    lv_label_set_text(ui_lblOpModeTZero, "0% temperature:");

    ui_lblOpModeTHundred = lv_label_create(ui_panOpMode);
    lv_obj_set_width(ui_lblOpModeTHundred, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_lblOpModeTHundred, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_y(ui_lblOpModeTHundred, 29);
    lv_obj_set_x(ui_lblOpModeTHundred, lv_pct(-7));
    lv_label_set_text(ui_lblOpModeTHundred, "100% temperature:");

    ui_lblOpModeTimebase = lv_label_create(ui_panOpMode);
    lv_obj_set_width(ui_lblOpModeTimebase, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_lblOpModeTimebase, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_y(ui_lblOpModeTimebase, 43);
    lv_obj_set_x(ui_lblOpModeTimebase, lv_pct(-7));
    lv_label_set_text(ui_lblOpModeTimebase, "Time base:");

    ui_btnMOre = lv_btn_create(ui_Home);
    lv_obj_set_width(ui_btnMOre, 80);
    lv_obj_set_height(ui_btnMOre, 30);
    lv_obj_set_x(ui_btnMOre, -12);
    lv_obj_set_y(ui_btnMOre, -50);
    lv_obj_set_align(ui_btnMOre, LV_ALIGN_BOTTOM_RIGHT);
    lv_obj_add_flag(ui_btnMOre, LV_OBJ_FLAG_SCROLL_ON_FOCUS);     /// Flags
    lv_obj_clear_flag(ui_btnMOre, LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    ui_lblMore = lv_label_create(ui_btnMOre);
    lv_obj_set_width(ui_lblMore, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_lblMore, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_lblMore, 0);
    lv_obj_set_y(ui_lblMore, 1);
    lv_obj_set_align(ui_lblMore, LV_ALIGN_CENTER);
    lv_label_set_text(ui_lblMore, "MORE");

    lv_obj_add_event_cb(ui_btnSetup, ui_event_btnSetup, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_btnMOre, ui_event_btnMOre, LV_EVENT_ALL, NULL);

}