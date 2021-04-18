/*
 * LED_pins.h
 *
 *  Created on: Aug 6, 2020
 *      Author: steph
 */

#ifndef LED_PINS_H_
#define LED_PINS_H_


#include "board_pin_defs.h"

#include <stdint.h>
#include <stdbool.h>
#include "driverlib/inc/hw_memmap.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"

static gpio_pin_t green_led = {
    .name       = "green",
    .pin        = GPIO_PIN_3,
    .peripheral = SYSCTL_PERIPH_GPIOB,
    .port       = GPIO_PORTB_AHB_BASE,
    .direction  = GPIO_DIR_MODE_OUT,
};

static gpio_pin_t blue_led = {
    .name       = "blue",
    .pin        = GPIO_PIN_4,
    .peripheral = SYSCTL_PERIPH_GPIOC,
    .port       = GPIO_PORTC_AHB_BASE,
    .direction  = GPIO_DIR_MODE_OUT,
};

static gpio_pin_t lcd_reset = {
    .name       = "lcd reset",
    .pin        = GPIO_PIN_0,
    .peripheral = SYSCTL_PERIPH_GPIOF,
    .port       = GPIO_PORTC_AHB_BASE,
    .direction  = GPIO_DIR_MODE_OUT,
};

static gpio_pin_t lcd_rs = {
    .name       = "lcd rs",
    .pin        = GPIO_PIN_4,
    .peripheral = SYSCTL_PERIPH_GPIOF,
    .port       = GPIO_PORTC_AHB_BASE,
    .direction  = GPIO_DIR_MODE_OUT,
};

static gpio_pin_t lcd_frame_signal = {
    .name       = "lcd fss",
    .pin        = GPIO_PIN_4,
    .peripheral = SYSCTL_PERIPH_GPIOA,
    .port       = GPIO_PORTA_AHB_BASE,
    .direction  = GPIO_DIR_MODE_OUT,
};

static gpio_pin_t* gpos[] = {
    &green_led,
    &blue_led,
    &lcd_reset,
    &lcd_frame_signal,
    &lcd_rs,
};

const GPOs_t led_info = {
    .GPOs     = gpos,
    .num_GPOs = sizeof(gpos)/sizeof(gpos[0]),
};

#endif /* LED_PINS_H_ */
