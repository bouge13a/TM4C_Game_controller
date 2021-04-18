/*
 * board_pins.h
 *
 *  Created on: Aug 4, 2020
 *      Author: steph
 */

#include "board_pin_defs.h"

#include <stdint.h>
#include <stdbool.h>
#include "driverlib/inc/hw_memmap.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"

static gpio_pin_t button_1 = {
    .name       = "Button A",
    .pin        = GPIO_PIN_6,
    .peripheral = SYSCTL_PERIPH_GPIOD,
    .port       = GPIO_PORTD_AHB_BASE,
    .direction  = GPIO_DIR_MODE_IN
};


static gpio_pin_t button_2 = { "Button B",
                               GPIO_PIN_7,
                               SYSCTL_PERIPH_GPIOD,
                               GPIO_PORTD_AHB_BASE,
                               GPIO_DIR_MODE_IN};

static gpio_pin_t select = {"Select",
                                   GPIO_PIN_4,
                                   SYSCTL_PERIPH_GPIOF,
                                   GPIO_PORTF_AHB_BASE,
                                   GPIO_DIR_MODE_IN};

static gpio_pin_t start = {"Start",
                                  GPIO_PIN_0,
                                  SYSCTL_PERIPH_GPIOF,
                                  GPIO_PORTF_AHB_BASE,
                                  GPIO_DIR_MODE_IN};

static gpio_pin_t joystick_ok = {"Joystick OK",
                                  GPIO_PIN_4,
                                  SYSCTL_PERIPH_GPIOE,
                                  GPIO_PORTE_AHB_BASE,
                                  GPIO_DIR_MODE_IN};

static gpio_pin_t* gpio_pins[] = {
    &button_1,
    &button_2,
    &select,
    &start,
    &joystick_ok
};

buttons_t button_info = {
    .buttons     = gpio_pins,
    .num_buttons = sizeof(gpio_pins)/sizeof(gpio_pins[0])
};


