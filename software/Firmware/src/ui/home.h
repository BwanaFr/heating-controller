
#ifndef _HOME_UI_H
#define _HOME_UI_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl.h"

extern lv_obj_t * ui_Home;
void ui_event_btnSetup(lv_event_t * e);
extern lv_obj_t * ui_btnSetup;
extern lv_obj_t * ui_lblSetup;
extern lv_obj_t * ui_panTempExt;
extern lv_obj_t * ui_lblTExt;
extern lv_obj_t * ui_lblTExtTitle;
extern lv_obj_t * ui_panTempLim;
extern lv_obj_t * ui_lblTLim;
extern lv_obj_t * ui_lblTLimTitle;
extern lv_obj_t * ui_panSetpoint;
extern lv_obj_t * ui_lblSetpoint;
extern lv_obj_t * ui_lblSetpointTitle;
extern lv_obj_t * ui_panOpMode;
extern lv_obj_t * ui_lblOpModeTitle;
extern lv_obj_t * ui_lblOpModeProfileTitle;
extern lv_obj_t * ui_lblOpModeTZero;
extern lv_obj_t * ui_lblOpModeTHundred;
extern lv_obj_t * ui_lblOpModeTimebase;
void ui_event_btnMOre(lv_event_t * e);
extern lv_obj_t * ui_btnMOre;
extern lv_obj_t * ui_lblMore;

void btnSetupClicked(lv_event_t * e);
void btnMoreClicked(lv_event_t * e);
void ui_Home_screen_init(void);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
