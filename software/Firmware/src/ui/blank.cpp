#include "blank.h"

#include "ui.h"
#ifndef SIMULATOR
#include "screen.h"
#include "inout.h"
#include <Arduino.h>
#endif
///////////////////// VARIABLES ////////////////////
lv_obj_t * ui_Blank;

void ui_event_BlankScreen(lv_event_t * e)
{
    ui_unblank_screen();
}

void ui_event_BlankScreenStyle(lv_event_t * e)
{
#ifndef SIMULATOR
    disable_lcd();
#endif
}

void ui_Blank_screen_init(void)
{
    ui_Blank = lv_obj_create(NULL);
    lv_obj_clear_flag(ui_Blank, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_add_event_cb(ui_Blank, ui_event_BlankScreen, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(ui_Blank, ui_event_BlankScreenStyle, LV_EVENT_SCREEN_LOADED, NULL);
#ifndef SIMULATOR
    lv_obj_add_event_cb(ui_Blank, [](lv_event_t * e){
          ledcWrite(0, 255);
    }, LV_EVENT_SCREEN_UNLOAD_START, NULL);
#endif    
}