// SquareLine LVGL GENERATED FILE
// EDITOR VERSION: SquareLine Studio 1.1.1
// LVGL VERSION: 8.2.0
// PROJECT: heating_controller

#include "ui.h"
#include "ui_helpers.h"

///////////////////// VARIABLES ////////////////////
lv_obj_t * ui_Home_screen;
lv_obj_t * ui_Temp_ext_panel;
lv_obj_t * ui_Home_screen_Label1;
lv_obj_t * ui_Temp_lim_panel;
lv_obj_t * ui_Temp_lim;
lv_obj_t * ui_Screen2;

///////////////////// TEST LVGL SETTINGS ////////////////////
#if LV_COLOR_DEPTH != 16
    #error "LV_COLOR_DEPTH should be 16bit to match SquareLine Studio's settings"
#endif
#if LV_COLOR_16_SWAP !=0
    #error "LV_COLOR_16_SWAP should be 0 to match SquareLine Studio's settings"
#endif

///////////////////// ANIMATIONS ////////////////////

///////////////////// FUNCTIONS ////////////////////

///////////////////// SCREENS ////////////////////
void ui_Home_screen_screen_init(void)
{
    ui_Home_screen = lv_obj_create(NULL);
    lv_obj_clear_flag(ui_Home_screen, LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    ui_Temp_ext_panel = lv_obj_create(ui_Home_screen);
    lv_obj_set_width(ui_Temp_ext_panel, 246);
    lv_obj_set_height(ui_Temp_ext_panel, 30);
    lv_obj_clear_flag(ui_Temp_ext_panel, LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    ui_Home_screen_Label1 = lv_label_create(ui_Temp_ext_panel);
    lv_obj_set_width(ui_Home_screen_Label1, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_Home_screen_Label1, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_Home_screen_Label1, LV_ALIGN_LEFT_MID);
    lv_label_set_text(ui_Home_screen_Label1, "Temperature ext. :");

    ui_Temp_lim_panel = lv_obj_create(ui_Home_screen);
    lv_obj_set_width(ui_Temp_lim_panel, 246);
    lv_obj_set_height(ui_Temp_lim_panel, 30);
    lv_obj_set_x(ui_Temp_lim_panel, 0);
    lv_obj_set_y(ui_Temp_lim_panel, -38);
    lv_obj_set_align(ui_Temp_lim_panel, LV_ALIGN_LEFT_MID);
    lv_obj_clear_flag(ui_Temp_lim_panel, LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    ui_Temp_lim = lv_label_create(ui_Temp_lim_panel);
    lv_obj_set_width(ui_Temp_lim, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_Temp_lim, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_Temp_lim, LV_ALIGN_LEFT_MID);
    lv_label_set_text(ui_Temp_lim, "Temperature limiteur :");

}
void ui_Screen2_screen_init(void)
{
    ui_Screen2 = lv_obj_create(NULL);
    lv_obj_clear_flag(ui_Screen2, LV_OBJ_FLAG_SCROLLABLE);      /// Flags

}

void ui_init(void)
{
    lv_disp_t * dispp = lv_disp_get_default();
    lv_theme_t * theme = lv_theme_default_init(dispp, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED),
                                               true, LV_FONT_DEFAULT);
    lv_disp_set_theme(dispp, theme);
    ui_Home_screen_screen_init();
    ui_Screen2_screen_init();
    lv_disp_load_scr(ui_Home_screen);
}
