#ifndef _UI_H
#define _UI_H

#include "lvgl.h"
#include "events.h"

/**
 * UI initialization 
*/
void ui_init(void);

/**
 * Shows home screen
*/
void ui_show_home(void);

/**
 * Shows the setup screen
*/
void ui_show_setup(void);

/**
 * Shows the more screen
*/
void ui_show_more(void);

/**
 * Unblank screen
*/
void ui_unblank_screen(void);

/**
 * Blanks screen
*/
void ui_blank_screen(void);

#endif