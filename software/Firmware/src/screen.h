#ifndef _SCREEN_H_
#define _SCREEN_H_

#include <cstdint>

/**
 * Setup the screen
*/
void setup_screen();

/**
 * Enable LCD screen
*/
void enable_lcd(void);

/**
 * Disables LCD screen
*/
void disable_lcd(void);

/**
 * Screen refresh loop
*/
void loop_screen();

/**
 * Signal the system is ready
*/
void set_system_ready();

#endif