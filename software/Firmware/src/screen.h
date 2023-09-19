#ifndef _SCREEN_H_
#define _SCREEN_H_

#include <cstdint>

/**
 * Setup the screen
*/
void setup_screen();

/**
 * Screen refresh loop
*/
void loop_screen(bool systemReady);

/**
 * Gets min/max loop times
*/
void get_loop_times(unsigned long &min, unsigned long &max);
#endif