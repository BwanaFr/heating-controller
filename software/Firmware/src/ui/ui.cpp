#include "ui.h"

#include "lvgl.h"
#include "blank.h"
#include "home.h"
#include "setup.h"
#include "events.h"

static lv_obj_t * prevScreen = NULL;

void ui_init(void)
{
    lv_disp_t * dispp = lv_disp_get_default();
    lv_theme_t * theme = lv_theme_default_init(dispp, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED),
                                               true, LV_FONT_DEFAULT);
    lv_disp_set_theme(dispp, theme);    
    ui_Home_screen_init();
    ui_Blank_screen_init();
    lv_disp_load_scr(ui_Home);
}

/**
 * Shows home screen
*/
void ui_show_home(void)
{
    lv_scr_load_anim(ui_Home, LV_SCR_LOAD_ANIM_FADE_ON, 500, 0, true);
}

/**
 * Shows the setup screen
*/
void ui_show_setup(void)
{
    ui_Setup_screen_init();
    lv_scr_load_anim(ui_Setup, LV_SCR_LOAD_ANIM_FADE_ON, 500, 0, false);
}

/**
 * Shows the more screen
*/
void ui_show_more(void)
{

}

void ui_unblank_screen(void)
{
    if(prevScreen){
        lv_scr_load_anim(prevScreen, LV_SCR_LOAD_ANIM_FADE_ON, 100, 0, false);
    }
}


void ui_blank_screen(void)
{
    if(lv_scr_act() != ui_Blank){
        prevScreen = lv_scr_act();
        lv_scr_load_anim(ui_Blank, LV_SCR_LOAD_ANIM_FADE_ON, 2000, 0, false);
    }
}