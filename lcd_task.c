/*
 * lcd_screen.c
 *
 *  Created on: Aug 7, 2020
 *      Author: steph
 */

#include <stdbool.h>
#include <stdint.h>
#include "driverlib/inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/ssi.h"
#include "driverlib/sysctl.h"

#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"

#include "lcd_task.h"
#include "GPOs.h"

static gpio_pin_t* frame_select;
static gpio_pin_t* lcd_reset;
static gpio_pin_t* lcd_rs;

static QueueHandle_t lcd_data_queue = NULL;
static QueueHandle_t lcd_cmd_queue = NULL;

void init_lcd_screen(void) {

    SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI2);

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);

    SysCtlGPIOAHBEnable(SYSCTL_PERIPH_GPIOB);

    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_SSI2));

    GPIOPinConfigure(GPIO_PB4_SSI2CLK);
    GPIOPinConfigure(GPIO_PB7_SSI2TX);

    GPIOPinTypeSSI(GPIO_PORTB_AHB_BASE, GPIO_PIN_7 | GPIO_PIN_4 );

    SSIConfigSetExpClk(SSI2_BASE,
                       SysCtlClockGet(),
                       SSI_FRF_MOTO_MODE_0,
                       SSI_MODE_MASTER,
                       1000000,
                       8);

    SSIEnable(SSI2_BASE);

    frame_select = get_gpo_config("lcd fss");
    lcd_reset    = get_gpo_config("lcd reset");
    lcd_rs       = get_gpo_config("lcd rs");

    set_gpo(frame_select, 0);
    set_gpo(lcd_reset, 1);
    set_gpo(lcd_rs, 1);

    lcd_data_queue = xQueueCreate(127*127, sizeof(uint8_t));
    lcd_cmd_queue = xQueueCreate(10, sizeof(uint8_t));

} // End init_lcd_screen

void put_lcd_data(uint8_t byte) {

    xQueueSend(lcd_data_queue, &byte, 0);

} // End put_lcd_byte

void put_lcd_cmd(uint8_t byte) {

    xQueueSend(lcd_cmd_queue, &byte, 0);

} // End put_lcd_cmd

void lcd_data_task(void* parm){

    uint8_t rx_char = 0;

    while(1) {

        xQueueReceive(lcd_data_queue, &rx_char, portMAX_DELAY);

        SSIDataPut(SSI2_BASE, rx_char);

        while(SSIBusy(SSI2_BASE)) {
            vTaskDelay(0);
        }

        vTaskDelay(0);

    }

} // End lcd_task

void lcd_cmd_task(void* parm){

    uint8_t rx_char = 0;

    while(1) {

        xQueueReceive(lcd_cmd_queue, &rx_char, portMAX_DELAY);

        set_gpo(lcd_rs, 0);

        SSIDataPut(SSI2_BASE, rx_char);

        while(SSIBusy(SSI2_BASE)) {
            vTaskDelay(0);
        }

        set_gpo(lcd_rs, 1);

        vTaskDelay(0);
    }

} // End lcd_task

void lcd_task(void* parm) {

    set_gpo(lcd_reset, 0);
    vTaskDelay(10);
    set_gpo(lcd_reset, 1);
    vTaskDelay(10);

    put_lcd_cmd(CM_SLPOUT);
    vTaskDelay(20);

    put_lcd_cmd(CM_GAMSET);
    vTaskDelay(1);
    put_lcd_data(0x04);
    vTaskDelay(1);

    put_lcd_cmd(CM_SETPWCTR);
    vTaskDelay(1);
    put_lcd_data(0x0A);
    put_lcd_data(0x14);
    vTaskDelay(1);

    put_lcd_cmd(CM_SETSTBA);
    vTaskDelay(1);
    put_lcd_data(0x0A);
    put_lcd_data(0x00);
    vTaskDelay(1);

    put_lcd_cmd(CM_SETSTBA);
    vTaskDelay(1);
    put_lcd_data(0x05);
    vTaskDelay(1);

    put_lcd_cmd(CM_COLMOD);
    vTaskDelay(1);
    put_lcd_data(0x05);
    vTaskDelay(1);

    put_lcd_cmd(CM_MADCTL);
    vTaskDelay(1);
    put_lcd_data(CM_MADCTL_BGR);
    vTaskDelay(1);

    put_lcd_cmd(CM_NORON);

    put_lcd_cmd(CM_CASET);
    vTaskDelay(1);
    put_lcd_data(0);
    put_lcd_data(2);
    put_lcd_data(129 >> 8);
    put_lcd_data(129);
    vTaskDelay(1);

    put_lcd_cmd(CM_RASET);
    vTaskDelay(1);
    put_lcd_data(0);
    put_lcd_data(3);
    put_lcd_data(130 >> 8);
    put_lcd_data(130);
    vTaskDelay(1);

    put_lcd_cmd(CM_RAMWR);
    vTaskDelay(1);

    int idx;
    for(idx=0; idx<16384; idx++) {
        put_lcd_data(0x0f);
        put_lcd_data(0x50);
        vTaskDelay(0);
    }

    vTaskDelay(1);
    put_lcd_cmd(CM_DISPON);

    vTaskSuspend(NULL);
    while(1) {

    }
} // End lcd_task
