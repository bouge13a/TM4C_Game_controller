/*
 * light_temp_task.c
 *
 *  Created on: Aug 7, 2020
 *      Author: steph
 */

#include <string.h>
#include <stdint.h>
#include <uartstdio.h>

#include "light_temp_task.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "I2C_task.h"
#include "text_controls.h"
#include "logger.h"

static const uint32_t TASK_INTERVAL_MS = 1000;

static const uint32_t LIGHT_VAL_COL = 20;

static i2c_msg_t light_i2c_msg;
static i2c_msg_t light_config_msg;
//static i2c_msg_t temp_i2c_msg;

static uint8_t light_rx_data[2];
static uint8_t light_tx_data[1];

static uint8_t light_config[3];

static void set_i2c_msgs(void) {

    light_i2c_msg.address      = 0x44;
    light_i2c_msg.bytes_rxed   = 0;
    light_i2c_msg.bytes_txed   = 0;
    light_i2c_msg.num_rx_bytes = 2;
    light_i2c_msg.num_tx_bytes = 1;
    light_i2c_msg.rx_data      = light_rx_data;
    light_i2c_msg.tx_data      = light_tx_data;

    light_tx_data[0] = 0x00;
    memset(light_rx_data, 0, 2);

    light_config_msg.address = 0x44;
    light_config_msg.bytes_rxed = 0;
    light_config_msg.bytes_txed = 0;
    light_config_msg.num_rx_bytes = 0;
    light_config_msg.num_tx_bytes = 3;
    light_config_msg.tx_data = light_config;
    light_config_msg.rx_data = NULL;

    light_config[0] = 0x01;
    light_config[1] = 0b11001100;
    light_config[2] = 0b00010000;

}

void init_light_temp(void) {

    set_i2c_msgs();
    add_i2c_msg(&light_i2c_msg);
}

void light_temp_task(void* parm) {

    set_i2c_msgs();
    add_i2c_msg(&light_config_msg);
    vTaskDelay(TASK_INTERVAL_MS);

    while(1) {

        set_i2c_msgs();
        add_i2c_msg(&light_i2c_msg);
        vTaskDelay(TASK_INTERVAL_MS);

    }

} // End light_temp_task

uint32_t get_light_value(void) {

    return ((light_rx_data[0] << 8) | light_rx_data[1]);

} // End get_light_value

void light_temp_drawpage(void) {

    cursor_pos(5, 0);
    UARTprintf("OPT3001 Ambient Light Sensor\r\n");
    UARTprintf("Value:");

}

void light_temp_drawdata(void) {

    cursor_pos(6, LIGHT_VAL_COL);
    UARTprintf("         ");
    cursor_pos(6, LIGHT_VAL_COL);
    UARTprintf("%d", get_light_value());

}

void light_temp_drawinput(int character) {

}
