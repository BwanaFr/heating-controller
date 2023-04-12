#include "setup.h"
#include "ui.h"
#include "ui_utils.h"
#include "events.h"
#include <cstdio>
#ifndef SIMULATOR
#include "global_info.h"
#endif
lv_obj_t * ui_Setup;


void ui_event_setup_loaded(lv_event_t * e)
{
/*#ifndef SIMULATOR    
    refresh_network_info();
    refresh_io_info();
#endif*/
}

/**
    Shows temperature dialog
    @param title Dialog title
    @param range_min Minimum range
    @param range_max Maximum range
    @param digit_count Number of digits
    @param separator_position Position of separator
    @param value Actual setpoint value
    @param cb Callback at user validation
**/
void showSetpointSetDialog(const char* title,  int32_t range_min,
                            int32_t range_max, uint8_t digit_count,
                            uint8_t separator_position, int32_t value,
                            void(*cb)(const int32_t&))
{
    // Box covering the whole screen (modal)
    lv_obj_t * mbox = lv_obj_create(lv_layer_top());
    lv_obj_set_style_bg_opa(mbox, 192, 0);
    lv_obj_set_height(mbox, LV_PCT(100));
    lv_obj_set_width(mbox, LV_PCT(100));         
    lv_obj_set_style_pad_all(mbox, 0, LV_PART_MAIN);
    lv_obj_set_style_border_width(mbox, 0, LV_PART_MAIN);   
    lv_obj_center(mbox);
    lv_obj_set_user_data(mbox, (void*)cb);

    //Content box to hold controls
    lv_obj_t * content = lv_obj_create(mbox);
    lv_obj_set_style_bg_opa(content, LV_OPA_COVER, 0);
    lv_obj_set_height(content, LV_PCT(90));
    lv_obj_set_width(content, LV_PCT(90));           
    lv_obj_set_style_pad_all(content, 0, LV_PART_MAIN);
    lv_obj_set_style_border_width(content, 0, LV_PART_MAIN);
    lv_obj_center(content);

    //Ok button
    lv_obj_t * btnOk = lv_btn_create(content);
    lv_obj_set_width(btnOk, 80);
    lv_obj_set_height(btnOk, 40);
    lv_obj_add_event_cb(btnOk, [](lv_event_t * e){
        lv_obj_t *btnOk = (lv_obj_t*)lv_event_get_target(e);
        lv_obj_t *spinbox = (lv_obj_t*)lv_obj_get_user_data(btnOk);
        lv_obj_t *mbox = (lv_obj_t*)lv_event_get_user_data(e);        
        void(*thisCB)(const int32_t&) = (void (*)(const int32_t&))lv_obj_get_user_data(mbox);
        (*thisCB)(lv_spinbox_get_value(spinbox));
        lv_obj_del(mbox);
    }, LV_EVENT_CLICKED, mbox);
    lv_obj_set_align(btnOk, LV_ALIGN_BOTTOM_LEFT);
    lv_obj_t * lblOk = lv_label_create(btnOk);
    lv_obj_set_width(lblOk, LV_SIZE_CONTENT);
    lv_obj_set_height(lblOk, LV_SIZE_CONTENT);
    lv_obj_set_align(lblOk, LV_ALIGN_CENTER);
    lv_label_set_text(lblOk, LV_SYMBOL_OK " Ok");

    //Cancel button
    lv_obj_t * btnCancel = lv_btn_create(content);
    lv_obj_set_width(btnCancel, 80);
    lv_obj_set_height(btnCancel, 40);
    lv_obj_add_event_cb(btnCancel, [](lv_event_t * e){
        lv_obj_t *mbox = (lv_obj_t*)lv_event_get_user_data(e);        
        lv_obj_del(mbox);
    }, LV_EVENT_CLICKED, mbox);
    lv_obj_set_align(btnCancel, LV_ALIGN_BOTTOM_RIGHT);
    lv_obj_t * lblCancel = lv_label_create(btnCancel);
    lv_obj_set_width(lblCancel, LV_SIZE_CONTENT);
    lv_obj_set_height(lblCancel, LV_SIZE_CONTENT);
    lv_obj_set_align(lblCancel, LV_ALIGN_CENTER);
    lv_label_set_text(lblCancel, LV_SYMBOL_CLOSE " Cancel");

    //Title
    lv_obj_t * lblTitle = lv_label_create(content);
    lv_obj_set_width(lblTitle, LV_SIZE_CONTENT);
    lv_obj_set_height(lblTitle, LV_SIZE_CONTENT);
    lv_obj_set_align(lblTitle, LV_ALIGN_TOP_MID);
    lv_label_set_text(lblTitle, title);

    //Spin box
    lv_obj_t * spinbox = lv_spinbox_create(content);
    lv_spinbox_set_range(spinbox, range_min, range_max);
    lv_spinbox_set_digit_format(spinbox, digit_count, separator_position);
    lv_spinbox_step_prev(spinbox);
    lv_spinbox_set_value(spinbox, value);
    lv_obj_set_width(spinbox, 100);
    lv_obj_center(spinbox);
    lv_spinbox_set_cursor_pos(spinbox, 0);
    lv_coord_t h = lv_obj_get_height(spinbox);
    lv_obj_set_user_data(btnOk, spinbox);   //Store a reference to the spinbox in the button object

    lv_obj_t * btn = lv_btn_create(content);
    lv_obj_set_size(btn, h, h);
    lv_obj_align_to(btn, spinbox, LV_ALIGN_OUT_RIGHT_MID, 5, 0);
    lv_obj_set_style_bg_img_src(btn, LV_SYMBOL_PLUS, 0);
    lv_obj_add_event_cb(btn, [](lv_event_t * e){
        lv_event_code_t code = lv_event_get_code(e);
        if(code == LV_EVENT_SHORT_CLICKED || code  == LV_EVENT_LONG_PRESSED_REPEAT) {
            lv_obj_t *src = (lv_obj_t*)lv_event_get_user_data(e);
            lv_spinbox_increment(src);
        }
    }, LV_EVENT_ALL, spinbox);

    btn = lv_btn_create(content);
    lv_obj_set_size(btn, h, h);
    lv_obj_align_to(btn, spinbox, LV_ALIGN_OUT_LEFT_MID, -5, 0);
    lv_obj_set_style_bg_img_src(btn, LV_SYMBOL_MINUS, 0);
    lv_obj_add_event_cb(btn, [](lv_event_t * e){
        lv_event_code_t code = lv_event_get_code(e);
        if(code == LV_EVENT_SHORT_CLICKED || code  == LV_EVENT_LONG_PRESSED_REPEAT) {
            lv_obj_t *src = (lv_obj_t*)lv_event_get_user_data(e);
            lv_spinbox_decrement(src);
        }
    }, LV_EVENT_ALL, spinbox);
}

/**
    Creates a temperature set control
**/
lv_obj_t* createSetPointButton(lv_obj_t* parent)
{
    lv_obj_t * ret = lv_btn_create(parent);
    lv_obj_set_width(ret, 80);
    lv_obj_set_height(ret, 30);
    lv_obj_set_style_border_width(ret, 1, LV_PART_MAIN);
    lv_obj_set_style_pad_all(ret, 0, LV_PART_MAIN);
    lv_obj_clear_flag(ret, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t * ui_lblBack = lv_label_create(ret);
    lv_obj_set_width(ui_lblBack, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_lblBack, LV_SIZE_CONTENT);
    lv_obj_set_align(ui_lblBack, LV_ALIGN_CENTER);
    lv_obj_set_user_data(ret, ui_lblBack);
    return ret;
}

/**
    Creates heater settings controls
**/
void create_heater_objects(lv_obj_t * tab)
{
    lv_obj_set_flex_flow(tab, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_all(tab, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_row(tab, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_column(tab, 0, LV_PART_MAIN);    
    lv_obj_t* profile = create_labeled_display(tab, "Heating profile :", lv_dropdown_create);
    lv_dropdown_set_options(profile, "Profile 1\nProfile 2\nOff");
    lv_obj_set_width(profile, 120);
    lv_obj_add_event_cb(profile, [](lv_event_t * e){
        lv_event_code_t code = lv_event_get_code(e);
        if(code == LV_EVENT_VALUE_CHANGED){
            char buf[32];
            lv_obj_t * obj = lv_event_get_target(e);
            uint16_t selected = lv_dropdown_get_selected(obj);
#ifdef SIMULATOR
            printf("Option: %u\n", selected);
#endif            
        }else if(code == LV_EVENT_MSG_RECEIVED){
#ifdef SIMULATOR            
            printf("New profile received!\n");
#endif            
        }
    }, LV_EVENT_ALL, NULL);
    lv_msg_subsribe_obj(EVT_NEW_HEATING_PROFILE, profile, NULL);

    //Profile zero temperature
    lv_obj_t* profileZeroTemp = create_labeled_display(tab, "0% temp :", createSetPointButton);
    lv_obj_set_width(profileZeroTemp, 120);
    lv_obj_add_event_cb(profileZeroTemp, [](lv_event_t * e){
        showSetpointSetDialog("0% (no-load) temperature [°C]", -400, 400, 3, 2, -50, [](const int32_t& val){
            printf("New Temp : %d\n", val);
            //TODO: Change process value
        });
    }, LV_EVENT_CLICKED, NULL);
    lv_obj_t* profileZeroTempLbl = (lv_obj_t *)lv_obj_get_user_data(profileZeroTemp);
    lv_obj_add_event_cb(profileZeroTempLbl, update_label_cb<double>, LV_EVENT_MSG_RECEIVED, lv_obj_get_parent(profileZeroTemp));
    lv_msg_subsribe_obj(EVT_NEW_ZERO_PC_TEMP, profileZeroTempLbl, (void *)"%.f°C");

    //Profile 100% temperature
    lv_obj_t* profile100Temp = create_labeled_display(tab, "100% temp :", createSetPointButton);
    lv_obj_set_width(profile100Temp, 120);
    lv_obj_add_event_cb(profile100Temp, [](lv_event_t * e){
        showSetpointSetDialog("100% (full-load) temperature", -400, 400, 3, 2, -100, [](const int32_t& val){
#ifdef SIMULATOR             
            printf("New 100% Temp : %d\n", val);
#endif            
            //TODO: Change process value
        });
    }, LV_EVENT_CLICKED, NULL);
    lv_obj_t* profile100TempLbl = (lv_obj_t *)lv_obj_get_user_data(profile100Temp);
    lv_obj_add_event_cb(profile100TempLbl, update_label_cb<double>, LV_EVENT_MSG_RECEIVED, lv_obj_get_parent(profile100Temp));
    lv_msg_subsribe_obj(EVT_NEW_HUNDRED_PC_TEMP, profile100TempLbl, (void *)"%.f°C");

    //Profile timebase
    lv_obj_t* profileTimebase = create_labeled_display(tab, "Time base :", createSetPointButton);
    lv_obj_set_width(profileTimebase, 120);
    lv_obj_add_event_cb(profileTimebase, [](lv_event_t * e){
        showSetpointSetDialog("Relay time base [s]", 10, 120, 3, 3, 10, [](const int32_t& val){
#ifdef SIMULATOR             
            printf("New timebase : %d\n", val);
            //TODO: Change process value
#endif            
        });
    }, LV_EVENT_CLICKED, NULL);
    lv_obj_t* profileTimebaseLbl = (lv_obj_t *)lv_obj_get_user_data(profileTimebase);
    lv_obj_add_event_cb(profileTimebaseLbl, update_label_cb<int32_t>, LV_EVENT_MSG_RECEIVED, lv_obj_get_parent(profileTimebase));
    lv_msg_subsribe_obj(EVT_NEW_TIME_BASE, profileTimebaseLbl, (void *)"%ds");
}

void ui_Setup_screen_init(void)
{
    ui_Setup = lv_obj_create(NULL);
    lv_obj_clear_flag(ui_Setup, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t * ui_btnBack = lv_btn_create(ui_Setup);
    lv_obj_set_width(ui_btnBack, 80);
    lv_obj_set_height(ui_btnBack, 32);
    lv_obj_set_align(ui_btnBack, LV_ALIGN_BOTTOM_MID);
    lv_obj_add_flag(ui_btnBack, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    lv_obj_clear_flag(ui_btnBack, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_event_cb(ui_btnBack, [](lv_event_t * e){
        ui_show_home();
    }, LV_EVENT_CLICKED, NULL);

    lv_obj_t * ui_lblBack = lv_label_create(ui_btnBack);
    lv_obj_set_width(ui_lblBack, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_lblBack, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_lblBack, LV_ALIGN_CENTER);
    lv_label_set_text(ui_lblBack, LV_SYMBOL_HOME " Back");

    lv_obj_t * tabview;
    tabview = lv_tabview_create(ui_Setup, LV_DIR_LEFT, 80);
    lv_obj_set_size(tabview, LV_PCT(100), 135);

    lv_obj_t * tab_btns = lv_tabview_get_tab_btns(tabview);
    lv_obj_set_style_radius(tab_btns, 5, LV_PART_MAIN);
    lv_obj_set_style_bg_color(tab_btns, lv_palette_darken(LV_PALETTE_GREY, 3), 0);

    /*Add 3 tabs (the tabs are page (lv_page) and can be scrolled*/
    lv_obj_t * tab1 = lv_tabview_add_tab(tabview, "Heating");
    create_heater_objects(tab1);
    lv_obj_t * tab2 = lv_tabview_add_tab(tabview, "I/O");
    lv_obj_t * tab3 = lv_tabview_add_tab(tabview, "MQTT");
    lv_obj_t * tab4 = lv_tabview_add_tab(tabview, "System");
    
    lv_obj_add_event_cb(ui_Setup, ui_event_setup_loaded, LV_EVENT_SCREEN_LOADED, NULL);
}