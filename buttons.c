/*
 * buttons.c
 *
 *  Created on: Aug 4, 2020
 *      Author: steph
 */

#ifndef BUTTONS_C_
#define BUTTONS_C_

#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <button_pins.h>
#include <buttons.h>
#include <uartstdio.h>

#include "text_controls.h"
#include "driverlib/inc/hw_gpio.h"
#include "driverlib/inc/hw_types.h"
#include "driverlib/inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"



///////////////////////////////////////////////////////////////////
//                      Buttons
//////////////////////////////////////////////////////////////////

void init_buttons(void) {

    uint32_t idx;

    for(idx=0; idx<button_info.num_buttons; idx++) {

        MAP_SysCtlPeripheralEnable(button_info.buttons[idx]->peripheral);

        SysCtlGPIOAHBEnable(button_info.buttons[idx]->peripheral);

        while(!SysCtlPeripheralReady(button_info.buttons[idx]->peripheral));

        // Unlock port so we can change it to a GPIO input
        // Once we have enabled (unlocked) the commit register then re-lock it
        // to prevent further changes.  PF0 is muxed with NMI thus a special case.
        HWREG(button_info.buttons[idx]->port + GPIO_O_LOCK) = GPIO_LOCK_KEY;
        HWREG(button_info.buttons[idx]->port + GPIO_O_CR) |= button_info.buttons[idx]->pin;
        HWREG(button_info.buttons[idx]->port + GPIO_O_LOCK) = 0;

        MAP_GPIODirModeSet(button_info.buttons[idx]->port,
                           button_info.buttons[idx]->pin,
                           button_info.buttons[idx]->direction);

        MAP_GPIOPadConfigSet(button_info.buttons[idx]->port,
                             button_info.buttons[idx]->pin,
                             GPIO_STRENGTH_2MA,
                             GPIO_PIN_TYPE_STD_WPU);
    }

} // End init_buttons

gpio_pin_t* get_button_config(const char* name) {

    int idx;

    for(idx=0; idx<button_info.num_buttons; idx++) {

        if(0 == strcmp(name, button_info.buttons[idx]->name)){
            return button_info.buttons[idx];
        }

    }

    assert(0);
    return NULL;
} // End get_button_config

uint32_t read_button(gpio_pin_t* config) {
    uint32_t value = MAP_GPIOPinRead(config->port, config->pin);

    return value >= 1 ? 0 : 1;
}

buttons_t get_button_struct(void) {
    return button_info;
}


#endif /* BUTTONS_C_ */
