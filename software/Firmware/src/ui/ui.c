#include "ui.h"

#include "lvgl.h"

void ui_init(void)
{
    lv_disp_t * dispp = lv_disp_get_default();
    lv_theme_t * theme = lv_theme_default_init(dispp, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED),
                                               true, LV_FONT_DEFAULT);
    lv_disp_set_theme(dispp, theme);
    ui_Home_screen_init();
    ui_Setup_screen_init();
    lv_disp_load_scr(ui_Home);
}

/**
 * Called when the setup button is clicked
*/
void btnSetupClicked(lv_event_t * e)
{
    lv_scr_load_anim(ui_Setup, LV_SCR_LOAD_ANIM_FADE_ON, 500, 0, false);
}

/**
 * Called when the more button is clicked
*/
void btnMoreClicked(lv_event_t * e)
{

}

/**
 * Called when the setup button is clicked
*/
void btnSetupBackClicked(lv_event_t * e)
{
    lv_scr_load_anim(ui_Home, LV_SCR_LOAD_ANIM_FADE_ON, 500, 0, false);
}
