
#ifndef _HOME_UI_H
#define _HOME_UI_H

#include "lvgl.h"

extern lv_obj_t * ui_Home;
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
extern lv_obj_t * ui_btnMore;
extern lv_obj_t * ui_lblMore;

void btnSetupClicked(lv_event_t * e);
void btnMoreClicked(lv_event_t * e);
void ui_Home_screen_init(void);


#endif
