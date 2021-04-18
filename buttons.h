/*
 * buttons.h
 *
 *  Created on: Aug 4, 2020
 *      Author: steph
 */

#ifndef BUTTONS_H_
#define BUTTONS_H_

#include "board_pin_defs.h"

// Button functions
void init_buttons(void);
gpio_pin_t* get_button_config(const char* name);
uint32_t read_button(gpio_pin_t* config);
buttons_t get_button_struct(void);

#endif /* BUTTONS_H_ */
