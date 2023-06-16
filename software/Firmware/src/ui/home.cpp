#include "home.h"

#include "events.h"
#include "ui.h"
#include "ui_utils.h"

///////////////////// VARIABLES ////////////////////
static lv_obj_t * ui_Home = NULL;

/**
    TODO: This function can be replaced by lambda
**/
void ui_event_btnSetup(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    if(event_code == LV_EVENT_CLICKED) {
       ui_show_setup();
    }
}

/**
    TODO: This function can be replaced by lambda
**/
void ui_event_btnMOre(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    if(event_code == LV_EVENT_CLICKED) {
        ui_show_more();
    }
}

/**
    TODO: This function can be replaced by lambda
**/
void update_setpoint_cb(lv_event_t *e){
    lv_obj_t *label = lv_event_get_target(e);
    lv_msg_t *m = lv_event_get_msg(e);

    const char *fmt = (const char *)lv_msg_get_user_data(m);
    const int32_t *v = (const int32_t *)lv_msg_get_payload(m);
    if(v == nullptr){
        lv_obj_add_flag(label, LV_OBJ_FLAG_HIDDEN);
    }else{
        lv_obj_clear_flag(label, LV_OBJ_FLAG_HIDDEN);
        if(*v == 0){
            lv_label_set_text(label, "OFF");
        }else{
            lv_label_set_text_fmt(label, fmt, *v);
        }
    }
}

/**
    Creates indicator to show temperature and set point
    @param parent Parent object
    @param title Indicator title
    @param format Value format string (printf like)
    @param event Message event to subscribe to
    @param event_cb Event callback to manage message
**/
lv_obj_t* create_indicator(lv_obj_t* parent, const char* title, const char* format, int event, lv_event_cb_t event_cb)
{
    lv_obj_t* ret = lv_obj_create(parent);
    lv_obj_set_width(ret, 100);
    lv_obj_set_height(ret, 75);
    lv_obj_clear_flag(ret, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t* lblValue = lv_label_create(ret);
    lv_obj_set_width(lblValue, LV_SIZE_CONTENT);
    lv_obj_set_height(lblValue, LV_SIZE_CONTENT);
    lv_obj_set_x(lblValue, 0);
    lv_obj_set_y(lblValue, 15);
    lv_obj_set_align(lblValue, LV_ALIGN_BOTTOM_MID);
    lv_label_set_text(lblValue, "---");
    lv_obj_set_style_text_align(lblValue, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(lblValue, &lv_font_montserrat_42, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_event_cb(lblValue, event_cb, LV_EVENT_MSG_RECEIVED, NULL);
    lv_msg_subsribe_obj(event, lblValue, (void *)format);

    lv_obj_t* lblTitle = lv_label_create(ret);
    lv_obj_set_width(lblTitle, LV_SIZE_CONTENT);
    lv_obj_set_height(lblTitle, LV_SIZE_CONTENT);
    lv_obj_set_x(lblTitle, 0);
    lv_obj_set_y(lblTitle, -5);
    lv_obj_set_align(lblTitle, LV_ALIGN_TOP_MID);
    lv_label_set_text(lblTitle, title);

    return ret;
}

lv_obj_t* create_button(lv_obj_t* parent, const char* title, lv_event_cb_t event_cb)
{
    lv_obj_t* ret = lv_btn_create(parent);
    lv_obj_set_width(ret, 94);
    lv_obj_set_height(ret, 36);
    // lv_obj_add_flag(ret, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    lv_obj_clear_flag(ret, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_event_cb(ret, event_cb, LV_EVENT_CLICKED, NULL);

    lv_obj_t* btnLabel = lv_label_create(ret);
    lv_obj_set_width(btnLabel, LV_SIZE_CONTENT);
    lv_obj_set_height(btnLabel, LV_SIZE_CONTENT);
    lv_obj_set_x(btnLabel, 0);
    lv_obj_set_y(btnLabel, 1);
    lv_obj_set_align(btnLabel, LV_ALIGN_CENTER);
    lv_label_set_text(btnLabel, title);
    return ret;
}


void ui_Home_screen_init(void)
{
    ui_Home = lv_obj_create(NULL);
    lv_obj_clear_flag(ui_Home, LV_OBJ_FLAG_SCROLLABLE);

//Setup button
    lv_obj_t* ui_btnSetup = create_button(ui_Home, LV_SYMBOL_SETTINGS " SETUP", ui_event_btnSetup);
    lv_obj_set_x(ui_btnSetup, -5);
    lv_obj_set_y(ui_btnSetup, -10);
    lv_obj_set_align(ui_btnSetup, LV_ALIGN_BOTTOM_RIGHT);

//More button    
    lv_obj_t* ui_btnMore = create_button(ui_Home, LV_SYMBOL_PLUS " MORE", ui_event_btnMOre);
    lv_obj_set_x(ui_btnMore, -5);
    lv_obj_set_y(ui_btnMore, -50);
    lv_obj_set_align(ui_btnMore, LV_ALIGN_BOTTOM_RIGHT);

//External temperature display
    lv_obj_t* ui_panTempExt = create_indicator(ui_Home, "T. out [°C]", "%.1f", EVT_NEW_EXT_TEMP, update_label_cb<double>);
    lv_obj_set_x(ui_panTempExt, 2);
    lv_obj_set_y(ui_panTempExt, 2);
    lv_obj_set_align(ui_panTempExt, LV_ALIGN_TOP_LEFT);

//Limiter temperature display
    lv_obj_t* ui_panTempLim = create_indicator(ui_Home, "T. floor [°C]", "%.1f", EVT_NEW_FLOOR_TEMP, update_label_cb<double>);
    lv_obj_set_x(ui_panTempLim, -2);
    lv_obj_set_y(ui_panTempLim, 2);
    lv_obj_set_align(ui_panTempLim, LV_ALIGN_TOP_RIGHT);
    //TODO: Properly destroy this subscription to make the home screen unloadable
    lv_msg_subscribe(EVT_NEW_LIMITER_STATE, [](void * s, lv_msg_t * msg){
        lv_obj_t* ui_panTempLim = (lv_obj_t*)lv_msg_get_user_data(msg);
        bool* active = (bool*)lv_msg_get_payload(msg);
        if(*active){
            lv_obj_set_style_bg_color(ui_panTempLim,  lv_palette_main(LV_PALETTE_RED), LV_PART_MAIN);
        }else{
            lv_obj_remove_local_style_prop(ui_panTempLim, LV_STYLE_BG_COLOR, LV_PART_MAIN);
        }
    }, ui_panTempLim);
    
//Process setpoint display
    lv_obj_t* ui_panSetpoint = create_indicator(ui_Home, "Setpoint", "%2d%%", EVT_NEW_SET_POINT, update_setpoint_cb);
    lv_obj_set_x(ui_panSetpoint, 0);
    lv_obj_set_y(ui_panSetpoint, 2);
    lv_obj_set_width(ui_panSetpoint, 110);
    lv_obj_set_align(ui_panSetpoint, LV_ALIGN_TOP_MID);


//Operating mode panel
    lv_obj_t* ui_panOpMode = lv_obj_create(ui_Home);
    lv_obj_set_style_pad_all(ui_panOpMode, 0, LV_PART_MAIN);
    lv_obj_set_width(ui_panOpMode, 212);
    lv_obj_set_height(ui_panOpMode, 85);
    lv_obj_set_x(ui_panOpMode, 3);
    lv_obj_set_y(ui_panOpMode, -3);
    lv_obj_set_align(ui_panOpMode, LV_ALIGN_BOTTOM_LEFT);
    lv_obj_clear_flag(ui_panOpMode, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_flex_flow(ui_panOpMode, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_all(ui_panOpMode, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_row(ui_panOpMode, 0, LV_PART_MAIN);
 
    lv_obj_t* ui_lblOpModeTitle = lv_label_create(ui_panOpMode);
    lv_obj_set_width(ui_lblOpModeTitle, lv_pct(100));
    lv_obj_set_height(ui_lblOpModeTitle, LV_SIZE_CONTENT);
    lv_obj_set_style_text_align(ui_lblOpModeTitle, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_text(ui_lblOpModeTitle, "Operating mode");

    create_info_display<const char *>(ui_panOpMode, "Current profile: ", "%s", EVT_NEW_HEATING_PROFILE);
    create_info_display<double>(ui_panOpMode, "0% temperature: ", "%.f°C", EVT_NEW_ZERO_PC_TEMP, true);
    create_info_display<double>(ui_panOpMode, "100% temperature: ", "%.f°C", EVT_NEW_HUNDRED_PC_TEMP, true);
    create_info_display<int>(ui_panOpMode, "Time base: ", "%ds", EVT_NEW_TIME_BASE, true);
}

lv_obj_t * ui_get_Home_screen(void)
{
    if(ui_Home == NULL){
        ui_Home_screen_init();
    }
    return ui_Home;
}