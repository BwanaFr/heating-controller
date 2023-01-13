#include "startup.h"
#include "ui.h"
#include "ui_utils.h"
#include "events.h"

lv_obj_t * ui_Startup;

void ui_Startup_screen_init(void)
{
    LV_IMG_DECLARE(heater);
    ui_Startup = lv_obj_create(NULL);
    lv_obj_clear_flag(ui_Startup, LV_OBJ_FLAG_SCROLLABLE);      /// Flags  
    lv_obj_t * ui_gif = lv_gif_create(ui_Startup);
    lv_obj_clear_flag(ui_gif, LV_OBJ_FLAG_SCROLLABLE);
    lv_gif_set_src(ui_gif, &heater);
    lv_obj_align(ui_gif, LV_ALIGN_CENTER, 0, 0);
}