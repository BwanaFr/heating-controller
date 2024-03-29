#include "setup.h"
#include "ui.h"
#include "ui_utils.h"
#include "events.h"
#include <cstdio>
#ifndef SIMULATOR
#include "parameters.h"
#endif

static lv_obj_t * ui_Setup = NULL;         //Reference to this (maybe useless)
lv_obj_t * setupTabview = NULL;     //Reference to the tabview for restoring selected tab
uint16_t selectedTab = 0;           //Selected tab before dialog confirmation
uint16_t profileEdited = 0;         //Currently edited profile
lv_obj_t * mbox = NULL;

static void btnBackCB(void * s, lv_msg_t * msg)
{
    if(mbox){
        lv_obj_del(mbox);
        lv_tabview_set_act(setupTabview, selectedTab, LV_ANIM_OFF);
        mbox = NULL;
    }else{
        ui_show_home();
        lv_msg_unsubscribe(s);
    }
}

void ui_event_setup_loaded(lv_event_t * e)
{
    lv_msg_subscribe(EVT_GO_BACK, btnBackCB, NULL);
}

/**
    Shows temperature dialog
    @param title Window title
    @param cb Callback when ok button is clicked
    @return content panel
**/
lv_obj_t* makeDialog(const char* title, lv_event_cb_t cb)
{
    // Save the actual active tab
    selectedTab = lv_tabview_get_tab_act(setupTabview);

    // Box covering the whole screen (modal)
    mbox = lv_obj_create(lv_layer_top());
    lv_obj_set_style_bg_opa(mbox, 192, 0);
    lv_obj_set_height(mbox, LV_PCT(100));
    lv_obj_set_width(mbox, LV_PCT(100));         
    lv_obj_set_style_pad_all(mbox, 0, LV_PART_MAIN);
    lv_obj_set_style_border_width(mbox, 0, LV_PART_MAIN);   
    lv_obj_center(mbox);
   
    //Content box to hold controls
    lv_obj_t * content = lv_obj_create(mbox);
    lv_obj_set_style_bg_opa(content, LV_OPA_COVER, 0);
    lv_obj_set_height(content, LV_PCT(90));
    lv_obj_set_width(content, LV_PCT(90));           
    lv_obj_set_style_pad_all(content, 0, LV_PART_MAIN);
    lv_obj_set_style_border_width(content, 0, LV_PART_MAIN);
    lv_obj_center(content);

    lv_event_cb_t closeCb = [](lv_event_t * e){
        lv_event_code_t code = lv_event_get_code(e);
        if(code == LV_EVENT_CLICKED) {
            lv_obj_t *mboxContent = (lv_obj_t*)lv_event_get_user_data(e);        
            lv_obj_t *targetMbox = lv_obj_get_parent(mboxContent);
            lv_obj_del(targetMbox);
            lv_tabview_set_act(setupTabview, selectedTab, LV_ANIM_OFF);
            mbox = NULL;
        }
    };

    //Ok button
    lv_obj_t * btnOk = lv_btn_create(content);
    lv_obj_set_width(btnOk, 80);
    lv_obj_set_height(btnOk, 40);
    lv_obj_add_event_cb(btnOk, cb, LV_EVENT_CLICKED, content);
    lv_obj_add_event_cb(btnOk, closeCb, LV_EVENT_CLICKED, content);
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
    lv_obj_add_event_cb(btnCancel, closeCb, LV_EVENT_CLICKED, content);
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
    return content;
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
    lv_obj_t * mboxContent = makeDialog(title, [](lv_event_t * e){
            lv_obj_t *targMboxContent = (lv_obj_t*)lv_event_get_user_data(e);
            lv_obj_t *spinbox = (lv_obj_t*)lv_obj_get_user_data(targMboxContent);
            void(*thisCB)(const int32_t&) = (void (*)(const int32_t&))lv_obj_get_user_data(spinbox);
            (*thisCB)(lv_spinbox_get_value(spinbox));
        });

    //Spin box
    lv_obj_t * spinbox = lv_spinbox_create(mboxContent);
    lv_spinbox_set_range(spinbox, range_min, range_max);
    lv_spinbox_set_digit_format(spinbox, digit_count, separator_position);
    lv_spinbox_step_prev(spinbox);
    lv_spinbox_set_value(spinbox, value);
    lv_obj_set_width(spinbox, 100);
    lv_obj_center(spinbox);
    lv_spinbox_set_cursor_pos(spinbox, 0);
    lv_coord_t h = lv_obj_get_height(spinbox);
    lv_obj_set_user_data(mboxContent, spinbox);   //Store a reference to the dialog content object
    lv_obj_set_user_data(spinbox, (void*)cb);   //Store callback in the spinner

    lv_obj_t * btn = lv_btn_create(mboxContent);
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

    btn = lv_btn_create(mboxContent);
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
    Creates a set point control
**/
lv_obj_t* createSetPointButton(lv_obj_t* parent)
{
    lv_obj_t * ret = lv_btn_create(parent);
    lv_obj_set_height(ret, 32);
    lv_obj_set_width(ret, 110);
    lv_obj_set_style_border_width(ret, 1, LV_PART_MAIN);
    lv_obj_set_style_pad_all(ret, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_bottom(ret, 3, LV_PART_MAIN);
    lv_obj_clear_flag(ret, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t * ui_lblBack = lv_label_create(ret);
    lv_obj_set_width(ui_lblBack, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_lblBack, LV_SIZE_CONTENT);
    lv_obj_set_align(ui_lblBack, LV_ALIGN_CENTER);
    return ret;
}

/**
    Creates heating global settings controls
**/
void create_heating_objects(lv_obj_t * tab)
{
    lv_obj_set_flex_flow(tab, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_all(tab, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_row(tab, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_column(tab, 0, LV_PART_MAIN);    
    lv_obj_t* profile = create_labeled_display(tab, "Current profile :", lv_dropdown_create);
    lv_dropdown_set_options_static(profile, "Off\nEco\nStandard");
    lv_obj_set_width(profile, 110);
    lv_obj_add_event_cb(profile, [](lv_event_t * e){
        lv_event_code_t code = lv_event_get_code(e);
        lv_obj_t * obj = lv_event_get_target(e);
        if(code == LV_EVENT_VALUE_CHANGED){                        
            char buf[32];
            lv_dropdown_get_selected_str(obj, buf, sizeof(buf));
#ifdef SIMULATOR
            printf("Selected profile : %s\n", buf);            
#else
            Parameters::getInstance()->setCurrentProfile(buf);
#endif            
        }else if(code == LV_EVENT_MSG_RECEIVED){
            lv_msg_t *m = lv_event_get_msg(e);
            const char *v = (const char *)lv_msg_get_payload(m);
            uint16_t selected = lv_dropdown_get_option_index(obj, v);
            lv_dropdown_set_selected(obj, selected);
        }
    }, LV_EVENT_ALL, NULL);
    lv_msg_subsribe_obj(EVT_NEW_HEATING_PROFILE, profile, NULL);
    lv_dropdown_set_selected(profile, Parameters::getInstance()->getCurrentProfileIndex());

//Profile timebase
    lv_obj_t* profileTimebase = create_labeled_display(tab, "Time base :", createSetPointButton);
    lv_obj_add_event_cb(profileTimebase, [](lv_event_t * e){
#ifdef SIMULATOR        
        showSetpointSetDialog("Relay time base [s]", 10, 120, 3, 3, 10, [](const int32_t& val){
            printf("New timebase : %d\n", val);
#else
        showSetpointSetDialog("Relay time base [s]", 10, 120, 3, 3, Parameters::getInstance()->getTimeBase(), [](const int32_t& val){
            Parameters::getInstance()->setTimeBase(val);
#endif
        });
    }, LV_EVENT_CLICKED, NULL);
#ifndef SIMULATOR    
    lv_obj_t* profileTimebaseLbl = (lv_obj_t *)lv_obj_get_child(profileTimebase, 0);
    lv_obj_add_event_cb(profileTimebaseLbl, update_label_cb<int32_t>, LV_EVENT_MSG_RECEIVED, lv_obj_get_parent(profileTimebase));
    lv_msg_subsribe_obj(EVT_NEW_TIME_BASE, profileTimebaseLbl, (void *)"%ds");
    lv_label_set_text_fmt(profileTimebaseLbl, "%ds", Parameters::getInstance()->getTimeBase());
#endif

//Limiter temperature
    lv_obj_t* limiterTemp = create_labeled_display(tab, "Max floor temp :", createSetPointButton);
    lv_obj_add_event_cb(limiterTemp, [](lv_event_t * e){
#ifdef SIMULATOR        
        showSetpointSetDialog("Max floor temp [°C]", 10, 120, 3, 3, 10, [](const int32_t& val){
            printf("New max floor temp : %d\n", val);
#else
        showSetpointSetDialog("Max floor temp [°C]", 10, 400, 3, 2, Parameters::getInstance()->getLimiterTemp()*10, [](const int32_t& val){
            double tempValue = val/10.0;
            Parameters::getInstance()->setLimiterTemp(tempValue);
#endif
        });
    }, LV_EVENT_CLICKED, NULL);
#ifndef SIMULATOR    
    lv_obj_t* limiterTempLbl = (lv_obj_t *)lv_obj_get_child(limiterTemp, 0);
    lv_obj_add_event_cb(limiterTempLbl, update_label_cb<int32_t>, LV_EVENT_MSG_RECEIVED, lv_obj_get_parent(limiterTemp));
    lv_msg_subsribe_obj(EVT_NEW_LIMITER_TEMP, limiterTempLbl, (void *)"%.1f°C");
    lv_label_set_text_fmt(limiterTempLbl, "%.1f°C", Parameters::getInstance()->getLimiterTemp());
#endif    
}

/**
    Creates profiles settings controls
**/
void create_profiles_objects(lv_obj_t * tab)
{
    static lv_obj_t* profilePTZeroTemp;
    static lv_obj_t* profilePT100Temp;
    static lv_obj_t* profileOTZeroTemp;
    static lv_obj_t* profileOT100Temp;

    lv_obj_set_flex_flow(tab, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_all(tab, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_row(tab, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_column(tab, 0, LV_PART_MAIN);    
    lv_obj_t* profile = create_labeled_display(tab, "Profile to edit :", lv_dropdown_create);
    lv_dropdown_set_options(profile, "Eco\nStandard");
    lv_obj_set_width(profile, 110);
    profileEdited = 0;
    lv_obj_add_event_cb(profile, [](lv_event_t * e){
        lv_event_code_t code = lv_event_get_code(e);
        if(code == LV_EVENT_VALUE_CHANGED){
            lv_obj_t * obj = lv_event_get_target(e);
            profileEdited = lv_dropdown_get_selected(obj);
#ifdef SIMULATOR
            printf("Option: %u\n", profileEdited);
#else
            lv_obj_t* profilePTZeroTempLbl = (lv_obj_t *)lv_obj_get_child(profilePTZeroTemp, 0);
            lv_obj_t* profilePT100TempLbl = (lv_obj_t *)lv_obj_get_child(profilePT100Temp, 0);
            lv_obj_t* profileOTZeroTempLbl = (lv_obj_t *)lv_obj_get_child(profileOTZeroTemp, 0);
            lv_obj_t* profileOT100TempLbl = (lv_obj_t *)lv_obj_get_child(profileOT100Temp, 0);
            double ptZeroTemp, ptFullTemp, otZeroTemp, otFullTemp = 0.0;
            Parameters::getInstance()->getProfileSetting(profileEdited, ptZeroTemp, ptFullTemp, 
                                                            otZeroTemp, otFullTemp);
            //Update labels and subscriptions according to choosen profile to edit
            lv_label_set_text_fmt(profilePTZeroTempLbl, "%.1f°C", ptZeroTemp);
            lv_msg_unsubscribe(lv_obj_get_user_data(profilePTZeroTempLbl));
            void* subID = lv_msg_subsribe_obj(profileEdited == 0 ? EVT_NEW_PROFILE_ECO_PT_OFF : EVT_NEW_PROFILE_STD_PT_OFF, profilePTZeroTempLbl, (void *)"%.1f°C");
            lv_obj_set_user_data(profilePTZeroTempLbl, subID);

            lv_label_set_text_fmt(profilePT100TempLbl, "%.1f°C", ptFullTemp);
            lv_msg_unsubscribe(lv_obj_get_user_data(profilePT100TempLbl));
            subID = lv_msg_subsribe_obj(profileEdited == 0 ? EVT_NEW_PROFILE_ECO_PT_FULL : EVT_NEW_PROFILE_STD_PT_FULL, profilePT100TempLbl, (void *)"%.1f°C");
            lv_obj_set_user_data(profilePT100TempLbl, subID);
            
            lv_label_set_text_fmt(profileOTZeroTempLbl, "%.1f°C", otZeroTemp);
            lv_msg_unsubscribe(lv_obj_get_user_data(profileOTZeroTempLbl));
            subID = lv_msg_subsribe_obj(profileEdited == 0 ? EVT_NEW_PROFILE_ECO_OPT_OFF : EVT_NEW_PROFILE_STD_OPT_OFF, profileOTZeroTempLbl, (void *)"%.1f°C");
            lv_obj_set_user_data(profileOTZeroTempLbl, subID);

            lv_label_set_text_fmt(profileOT100TempLbl, "%.1f°C", otFullTemp);
            lv_msg_unsubscribe(lv_obj_get_user_data(profileOT100TempLbl));
            subID = lv_msg_subsribe_obj(profileEdited == 0 ? EVT_NEW_PROFILE_ECO_OPT_FULL : EVT_NEW_PROFILE_STD_OPT_FULL, profileOT100TempLbl, (void *)"%.1f°C");
            lv_obj_set_user_data(profileOT100TempLbl, subID);
#endif            
        }
    }, LV_EVENT_VALUE_CHANGED, NULL);

//Profile zero temperature at peak time
    profilePTZeroTemp = create_labeled_display(tab, "T 0% peak:", createSetPointButton);
    lv_obj_add_event_cb(profilePTZeroTemp, [](lv_event_t * e){
#ifdef SIMULATOR
        showSetpointSetDialog("0% (no-load) temperature [°C]\nPeak time", -400, 400, 3, 2, 123, [](const int32_t& val){
            printf("New Temp : %d\n", val);
#else
        double minVal = profileEdited == 0 ? Parameters::getInstance()->getProfileEcoPeakFullTemp() : Parameters::getInstance()->getProfileStdPeakFullTemp();
        showSetpointSetDialog("0% (no-load) temperature [°C]\nPeak time", minVal*10, 400, 3, 2, Parameters::getInstance()->getProfileEcoPeakOffTemp()*10, [](const int32_t& val){
            double tempValue = val/10.0;
            Parameters::getInstance()->setProfilePeakOffTemp(profileEdited, tempValue);
#endif
        });
    }, LV_EVENT_CLICKED, NULL);
#ifndef SIMULATOR
    lv_obj_t* profilePTZeroTempLbl = (lv_obj_t *)lv_obj_get_child(profilePTZeroTemp, 0);
    lv_obj_add_event_cb(profilePTZeroTempLbl, update_label_cb<double>, LV_EVENT_MSG_RECEIVED, lv_obj_get_parent(profilePTZeroTemp));
    void* subID = lv_msg_subsribe_obj(EVT_NEW_PROFILE_ECO_PT_OFF, profilePTZeroTempLbl, (void *)"%.1f°C");
    lv_obj_set_user_data(profilePTZeroTempLbl, subID);
    lv_label_set_text_fmt(profilePTZeroTempLbl, "%.1f°C", Parameters::getInstance()->getProfileEcoPeakOffTemp());
#endif

//Profile 100% temperature at peak time
    profilePT100Temp = create_labeled_display(tab, "T 100% peak:", createSetPointButton);
    lv_obj_add_event_cb(profilePT100Temp, [](lv_event_t * e){
#ifdef SIMULATOR        
        showSetpointSetDialog("100% (full-load) temperature\nPeak time", -400, 400, 3, 2, 456, [](const int32_t& val){
            printf("New 100%% Temp : %d\n", val);
#else
        double maxVal = profileEdited == 0 ? Parameters::getInstance()->getProfileEcoPeakOffTemp() : Parameters::getInstance()->getProfileStdPeakOffTemp();
        showSetpointSetDialog("100% (full-load) temperature\nPeak time", -400, maxVal*10, 3, 2, Parameters::getInstance()->getProfileEcoPeakFullTemp()*10, [](const int32_t& val){
            double tempValue = val/10.0;
            Parameters::getInstance()->setProfilePeakFullTemp(profileEdited, tempValue);
#endif
        });
    }, LV_EVENT_CLICKED, NULL);
#ifndef SIMULATOR
    lv_obj_t* profilePT100TempLbl = (lv_obj_t *)lv_obj_get_child(profilePT100Temp, 0);
    lv_obj_add_event_cb(profilePT100TempLbl, update_label_cb<double>, LV_EVENT_MSG_RECEIVED, lv_obj_get_parent(profilePT100TempLbl));
    subID = lv_msg_subsribe_obj(EVT_NEW_PROFILE_ECO_PT_FULL, profilePT100TempLbl, (void *)"%.1f°C");
    lv_obj_set_user_data(profilePT100TempLbl, subID);
    lv_label_set_text_fmt(profilePT100TempLbl, "%.1f°C", Parameters::getInstance()->getProfileEcoPeakFullTemp());
#endif

//Profile zero temperature at off-peak time
    profileOTZeroTemp = create_labeled_display(tab, "T 0% off-peak:", createSetPointButton);
    lv_obj_add_event_cb(profileOTZeroTemp, [](lv_event_t * e){
#ifdef SIMULATOR        
        showSetpointSetDialog("0% (no-load) temperature [°C]\nOff-peak time", -400, 400, 3, 2, 789, [](const int32_t& val){
            printf("New Temp : %d\n", val);
#else
        double minVal = profileEdited == 0 ? Parameters::getInstance()->getProfileEcoOffPeakFullTemp() : Parameters::getInstance()->getProfileStdOffPeakFullTemp();
        showSetpointSetDialog("0% (no-load) temperature [°C]\nOff-peak time", minVal*10, 400, 3, 2, Parameters::getInstance()->getProfileEcoOffPeakOffTemp()*10, [](const int32_t& val){
            double tempValue = val/10.0;
            Parameters::getInstance()->setProfileOffPeakOffTemp(profileEdited, tempValue);
#endif
        });
    }, LV_EVENT_CLICKED, NULL);
#ifndef SIMULATOR
    lv_obj_t* profileOTZeroTempLbl = (lv_obj_t *)lv_obj_get_child(profileOTZeroTemp, 0);
    lv_obj_add_event_cb(profileOTZeroTempLbl, update_label_cb<double>, LV_EVENT_MSG_RECEIVED, lv_obj_get_parent(profileOTZeroTempLbl));
    subID = lv_msg_subsribe_obj(EVT_NEW_PROFILE_ECO_OPT_OFF, profileOTZeroTempLbl, (void *)"%.1f°C");
    lv_obj_set_user_data(profileOTZeroTempLbl, subID);
    lv_label_set_text_fmt(profileOTZeroTempLbl, "%.1f°C", Parameters::getInstance()->getProfileEcoOffPeakOffTemp());
#endif

//Profile 100% temperature at off-peak time
    profileOT100Temp = create_labeled_display(tab, "T 100% off-peak:", createSetPointButton);
    lv_obj_add_event_cb(profileOT100Temp, [](lv_event_t * e){
#ifdef SIMULATOR
        showSetpointSetDialog("100% (full-load) temperature\nOff-peak time", -400, 400, 3, 2, 1122, [](const int32_t& val){
            printf("New 100%% Temp : %d\n", val);
#else
        double maxVal = profileEdited == 0 ? Parameters::getInstance()->getProfileEcoOffPeakOffTemp() : Parameters::getInstance()->getProfileStdOffPeakOffTemp();
        showSetpointSetDialog("100% (full-load) temperature\nOff-peak time", -400, maxVal*10, 3, 2, Parameters::getInstance()->getProfileEcoOffPeakFullTemp()*10, [](const int32_t& val){
            double tempValue = val/10.0;
            Parameters::getInstance()->setProfileOffPeakFullTemp(profileEdited, tempValue);
#endif
        });
    }, LV_EVENT_CLICKED, NULL);
#ifndef SIMULATOR
    lv_obj_t* profileOT100TempLbl = (lv_obj_t *)lv_obj_get_child(profileOT100Temp, 0);
    lv_obj_add_event_cb(profileOT100TempLbl, update_label_cb<double>, LV_EVENT_MSG_RECEIVED, lv_obj_get_parent(profileOT100TempLbl));
    subID = lv_msg_subsribe_obj(EVT_NEW_PROFILE_ECO_PT_FULL, profileOT100TempLbl, (void *)"%.1f°C");
    lv_obj_set_user_data(profileOT100TempLbl, subID);
    lv_label_set_text_fmt(profileOT100TempLbl, "%.1f°C", Parameters::getInstance()->getProfileEcoOffPeakFullTemp());
#endif    
}

/**
    Creates I/O settings controls
**/
void create_io_settings_objects(lv_obj_t * tab)
{
    lv_obj_set_flex_flow(tab, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_all(tab, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_row(tab, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_column(tab, 0, LV_PART_MAIN);    

//ADC reference voltage
    lv_obj_t* adcRefVoltage = create_labeled_display(tab, "ADC ref voltage:", createSetPointButton);
    lv_obj_add_event_cb(adcRefVoltage, [](lv_event_t * e){
#ifndef SIMULATOR        
        showSetpointSetDialog("ADC reference voltage [mV]", 0, 3500, 4, 4, Parameters::getInstance()->getADCRefVoltage(), [](const int32_t& val){
            Parameters::getInstance()->setADCRefVoltage(val);
        });
#endif        
    }, LV_EVENT_CLICKED, NULL);
#ifndef SIMULATOR
    lv_obj_t* adcRefVoltageLbl = (lv_obj_t *)lv_obj_get_child(adcRefVoltage, 0);
    lv_obj_add_event_cb(adcRefVoltageLbl, update_label_cb<int>, LV_EVENT_MSG_RECEIVED, lv_obj_get_parent(adcRefVoltageLbl));
    lv_msg_subsribe_obj(EVT_NEW_ADC_REF_VOLTAGE, adcRefVoltageLbl, (void *)"%dmV");
    lv_label_set_text_fmt(adcRefVoltageLbl, "%dmV", Parameters::getInstance()->getADCRefVoltage());
#endif

//ADC averaging
    lv_obj_t* adcAveraging = create_labeled_display(tab, "ADC averaging:", createSetPointButton);
    lv_obj_add_event_cb(adcAveraging, [](lv_event_t * e){
#ifndef SIMULATOR        
        showSetpointSetDialog("ADC averaging [samples]", 0, 500, 4, 4, Parameters::getInstance()->getADCAveraging(), [](const int32_t& val){
            Parameters::getInstance()->setADCAverage(val);
        });
#endif        
    }, LV_EVENT_CLICKED, NULL);
#ifndef SIMULATOR
    lv_obj_t* adcAveragingLbl = (lv_obj_t *)lv_obj_get_child(adcAveraging, 0);
    lv_obj_add_event_cb(adcAveragingLbl, update_label_cb<int>, LV_EVENT_MSG_RECEIVED, lv_obj_get_parent(adcAveragingLbl));
    lv_msg_subsribe_obj(EVT_NEW_ADC_AVERAGING, adcAveragingLbl, (void *)"%d");
    lv_label_set_text_fmt(adcAveragingLbl, "%d", Parameters::getInstance()->getADCAveraging());
#endif
}

/**
    Creates system settings controls
**/
void create_system_settings_objects(lv_obj_t * tab)
{
    lv_obj_set_flex_flow(tab, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_all(tab, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_row(tab, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_column(tab, 0, LV_PART_MAIN);    

//Reset settings
    lv_obj_t* resetSettings = create_labeled_display(tab, "Reset settings:", createSetPointButton);
    lv_obj_add_event_cb(resetSettings, [](lv_event_t * e){
        makeDialog("Reset settings to factory default?\nAll parameters will be lost!", [](lv_event_t * evt){
#ifdef SIMULATOR
            printf("Reseting settings\n");
#else
            Parameters::getInstance()->resetToDefaults();            
#endif
        });
    }, LV_EVENT_CLICKED, NULL);
    lv_obj_t* resetSettingsLbl = (lv_obj_t *)lv_obj_get_child(resetSettings, 0);
    lv_label_set_text(resetSettingsLbl, "Reset");
}

void create_back_objects(lv_obj_t * tab)
{
    lv_obj_t * ui_btnBack = lv_btn_create(tab);
    lv_obj_set_width(ui_btnBack, 120);
    lv_obj_set_height(ui_btnBack, 80);
    lv_obj_set_align(ui_btnBack, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_btnBack, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    lv_obj_clear_flag(ui_btnBack, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_event_cb(ui_btnBack, [](lv_event_t * e){
        lv_event_code_t code = lv_event_get_code(e);
        if(code == LV_EVENT_CLICKED) {
            ui_show_home();
        }
    }, LV_EVENT_CLICKED, NULL);

    lv_obj_t * ui_lblBack = lv_label_create(ui_btnBack);
    lv_obj_set_width(ui_lblBack, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_lblBack, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_lblBack, LV_ALIGN_CENTER);
    lv_label_set_text(ui_lblBack, LV_SYMBOL_HOME " Back");
}

void ui_Setup_screen_init(void)
{
    ui_Setup = lv_obj_create(NULL);
    lv_obj_clear_flag(ui_Setup, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_pad_all(ui_Setup, 0, LV_PART_MAIN);

    setupTabview = lv_tabview_create(ui_Setup, LV_DIR_LEFT, 80);
    lv_obj_set_size(setupTabview, LV_PCT(100), LV_PCT(100));

    lv_obj_t * tab_btns = lv_tabview_get_tab_btns(setupTabview);
    lv_obj_set_style_radius(tab_btns, 2, LV_PART_MAIN);
    lv_obj_set_style_bg_color(tab_btns, lv_palette_darken(LV_PALETTE_GREY, 3), 0);
    
    /*Add 3 tabs (the tabs are page (lv_page) and can be scrolled*/
    lv_obj_t * tab1 = lv_tabview_add_tab(setupTabview, "Heating");
    create_heating_objects(tab1);
    lv_obj_t * tab2 = lv_tabview_add_tab(setupTabview, "Profiles");
    create_profiles_objects(tab2);
    lv_obj_t * tab3 = lv_tabview_add_tab(setupTabview, "I/O");
    create_io_settings_objects(tab3);
    lv_obj_t * tab4 = lv_tabview_add_tab(setupTabview, "System");
    create_system_settings_objects(tab4);
    lv_obj_t * tab5 = lv_tabview_add_tab(setupTabview, "Back");
    create_back_objects(tab5);

    lv_obj_add_event_cb(ui_Setup, ui_event_setup_loaded, LV_EVENT_SCREEN_LOADED, NULL);
}

lv_obj_t * ui_get_Setup_screen(void)
{
    if(ui_Setup == NULL){
        ui_Setup_screen_init();
    }
    return ui_Setup;
}