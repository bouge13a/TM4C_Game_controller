/*
 * control_page.c
 *
 *  Created on: Aug 5, 2020
 *      Author: steph
 */

#include <stdint.h>
#include <uartstdio.h>

#include "control_page.h"
#include "buttons.h"
#include "board_pin_defs.h"
#include "text_controls.h"
#include "ADC_task.h"

buttons_t buttons_info;
adc_pins_t adc_info;

static const uint32_t BUTTON_VAL_COL = 20;
static const uint32_t ADC_NAME_COL   = 30;
static const uint32_t ADC_VAL_COL    = 50;

///////////////////////////////////////////////////////////////////////
//                        Console Page
///////////////////////////////////////////////////////////////////////
void control_drawpage(void) {

    int idx;

    buttons_info = get_button_struct();
    adc_info     = get_adc_struct();

    cursor_pos(5, 0);
    set_text_mode(mode_underline);
    UARTprintf("Buttons\r\n");
    set_text_mode(mode_reset);

    text_color(green_text);
    for(idx=0; idx<buttons_info.num_buttons; idx++) {
        UARTprintf("%s\r\n", buttons_info.buttons[idx]->name);
    }

    set_text_mode(mode_reset);

    cursor_pos(5, ADC_NAME_COL);
    set_text_mode(mode_underline);
    UARTprintf("ADCs");
    set_text_mode(mode_reset);


    text_color(magenta_text);
    for(idx=0; idx<adc_info.num_adc_pins; idx++) {
        cursor_pos(6+idx, ADC_NAME_COL);
        UARTprintf("%s", adc_info.adc_pins[idx]->name);
    }

    set_text_mode(mode_reset);
}

void control_drawdata(void) {

    int idx;

    for(idx=0; idx<buttons_info.num_buttons; idx++){
        cursor_pos(6+idx, BUTTON_VAL_COL);
        UARTprintf("%d", read_button(buttons_info.buttons[idx]));
    }

    for(idx=0; idx<adc_info.num_adc_pins; idx++){
        cursor_pos(6+idx, ADC_VAL_COL);
        UARTprintf("    ");
        cursor_pos(6+idx, ADC_VAL_COL);
        UARTprintf("%d", get_adc_val(adc_info.adc_pins[idx]));
    }

} // End control_drawdata

void control_drawinput(int character) {

}
