// SquareLine LVGL GENERATED FILE
// EDITOR VERSION: SquareLine Studio 1.1.1
// LVGL VERSION: 8.2.0
// PROJECT: display

#ifndef _SETUP_UI_H
#define _SETUP_UI_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl.h"

extern lv_obj_t * ui_Setup;
void ui_event_btnBackMain(lv_event_t * e);
extern lv_obj_t * ui_btnBackMain;
extern lv_obj_t * ui_lblBackMain;
extern lv_obj_t * ui_setupPanel;
extern lv_obj_t * ui_netPanel;
extern lv_obj_t * ui_btnNetwork;
extern lv_obj_t * ui_netLabel;
extern lv_obj_t * ui_heatPanel;
extern lv_obj_t * ui_btnHeater;
extern lv_obj_t * ui_heatLabel;


LV_IMG_DECLARE(ui_img_heat_pump_png);
LV_IMG_DECLARE(ui_img_network_png);


void btnSetupBackClicked(lv_event_t * e);
void ui_Setup_screen_init(void);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
