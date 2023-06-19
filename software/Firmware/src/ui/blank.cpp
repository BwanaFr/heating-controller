#include "blank.h"

#include "ui.h"
#ifndef SIMULATOR
#include "screen.h"
#include "inout.h"
#include <Arduino.h>
#endif
///////////////////// VARIABLES ////////////////////
lv_obj_t * ui_Blank;

void ui_Blank_screen_init(void)
{
    ui_Blank = lv_obj_create(NULL);
    lv_obj_clear_flag(ui_Blank, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
#ifndef SIMULATOR    
    lv_obj_add_event_cb(ui_Blank, [](lv_event_t * e){ disable_lcd(); }, LV_EVENT_SCREEN_LOADED, NULL);
    lv_obj_add_event_cb(ui_Blank, [](lv_event_t * e){ enable_lcd();  }, LV_EVENT_SCREEN_UNLOAD_START, NULL);
#endif    
}