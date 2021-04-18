/*
 * startup_task.c
 *
 *  Created on: Aug 6, 2020
 *      Author: steph
 */

#include "FreeRTOS.h"
#include "task.h"

#include "board_task.h"
#include "GPOs.h"
#include "buzzer_lcdbl.h"
#include "board_pin_defs.h"
#include "light_temp_task.h"
#include "lcd_task.h"

static gpio_pin_t* green_led;
static gpio_pin_t* blue_led;

static const uint32_t STARTUP_PERIOD_MS = 3000;
static const uint32_t FREQ_INC          = 1;
static const uint32_t START_FREQ        = 1000;
static const uint32_t LED_TOG_PERIOD    = 50;

void init_board(void) {

    green_led = get_gpo_config("green");
    blue_led  = get_gpo_config("blue");
}

void board_task(void* parm) {

    // loop index
    int idx;

    // Toggle the LEDs and ramp up the buzzer
    for(idx=0; idx<STARTUP_PERIOD_MS; idx++) {

        if (0 == idx % LED_TOG_PERIOD) {
            if (0 == get_gpo(green_led)) {
                set_gpo(green_led, 1);
                set_gpo(blue_led, 0);
            } else {
                set_gpo(green_led, 0);
                set_gpo(blue_led, 1);
            }
        }

        set_buzzer_period(FREQ_INC*idx + START_FREQ);

        vTaskDelay(1);
    }

    set_gpo(green_led, 0);
    set_gpo(blue_led, 0);

    stop_buzzer();

    while(1) {

        if(get_light_value() > 30000) {
            set_lcdbl_duty_period(3000);
        } else {
            set_lcdbl_duty_period(get_light_value()/10);
        }
        vTaskDelay(1000);
    }

} // End startup_task

