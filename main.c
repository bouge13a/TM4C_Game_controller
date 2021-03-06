

/**
 * main.c
 */

#include <buttons.h>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include <stdbool.h>

#include "driverlib/debug.h"
#include "driverlib/fpu.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"

#include "console_task.h"
#include "homepage.h"
#include "console_uart.h"
#include "control_page.h"
#include "ADC_task.h"
#include "GPOs.h"
#include "board_task.h"
#include "light_temp_task.h"
#include "I2C_task.h"
#include "logger.h"
#include "buzzer_lcdbl.h"
#include "task_manager_page.h"
#include "lcd_task.h"
#include "usb_task.h"

static QueueHandle_t console_uart_rx_q = NULL;

int main(void){

    // Enable lazy stacking for interrupt handlers.  This allows floating-point
    // instructions to be used within interrupt handlers, but at the expense of
    // extra stack usage.
    MAP_FPUEnable();
    MAP_FPULazyStackingEnable();

    // Set the system clock to 50 MHz
    MAP_SysCtlClockSet(SYSCTL_SYSDIV_4 |
                       SYSCTL_USE_PLL |
                       SYSCTL_OSC_MAIN |
                       SYSCTL_XTAL_16MHZ);

    /////////////////////////////////////////////////////////
    //           Create Queues and Semaphores
    /////////////////////////////////////////////////////////

    console_uart_rx_q = xQueueCreate(10, sizeof(uint8_t));

    /////////////////////////////////////////////////////////
    //                    Initialization
    /////////////////////////////////////////////////////////

    init_console_uart(console_uart_rx_q);

    init_console(console_uart_rx_q);

    init_buttons();

    init_adc();

    init_gpos();

    init_buzzer_lcdbl();

    init_board();

    init_i2c();

    init_light_temp();

    init_lcd_screen();

    init_usb();


    // Enable processor interrupts.
    IntMasterEnable();

    /////////////////////////////////////////////////////////
    //                  Add pages to Console
    /////////////////////////////////////////////////////////

    add_page("Homepage1",
             homepage_drawpage,
             homepage_drawdata,
             homepage_drawinput,
             portMAX_DELAY);

    add_page("Controls",
             control_drawpage,
             control_drawdata,
             control_drawinput,
             250);

    add_page("Light & Temp",
             light_temp_drawpage,
             light_temp_drawdata,
             light_temp_drawinput,
             1000);

    add_page("Logger",
             logger_drawpage,
             logger_drawdata,
             logger_drawinput,
             1000);

    add_page("Task Manager",
             taskmanager_drawpage,
             taskmanager_drawdata,
             taskmanager_drawinput,
             portMAX_DELAY);

    ///////////////////////////////////////////////////////
    //                Create Tasks
    ///////////////////////////////////////////////////////

    xTaskCreate(console_task,               /* Function that implements the task. */
                "Console",                  /* Text name for the task. */
                220,                        /* Stack size in words, not bytes. */
                NULL,                       /* Parameter passed into the task. */
                1,                          /* Priority at which the task is created. */
                NULL );                     /* Used to pass out the created task's handle. */

    xTaskCreate(adc_task,               /* Function that implements the task. */
                "ADC",                  /* Text name for the task. */
                configMINIMAL_STACK_SIZE,                        /* Stack size in words, not bytes. */
                NULL,                       /* Parameter passed into the task. */
                2,                          /* Priority at which the task is created. */
                NULL );                     /* Used to pass out the created task's handle. */

    xTaskCreate(board_task,               /* Function that implements the task. */
                "Board",                  /* Text name for the task. */
                configMINIMAL_STACK_SIZE,                        /* Stack size in words, not bytes. */
                NULL,                       /* Parameter passed into the task. */
                2,                          /* Priority at which the task is created. */
                NULL );                     /* Used to pass out the created task's handle. */

    xTaskCreate(light_temp_task,               /* Function that implements the task. */
                "Light Temp",                  /* Text name for the task. */
                configMINIMAL_STACK_SIZE,                        /* Stack size in words, not bytes. */
                NULL,                       /* Parameter passed into the task. */
                2,                          /* Priority at which the task is created. */
                NULL );                     /* Used to pass out the created task's handle. */

    xTaskCreate(i2c_task,               /* Function that implements the task. */
                "I2C1",                  /* Text name for the task. */
                configMINIMAL_STACK_SIZE,                        /* Stack size in words, not bytes. */
                NULL,                       /* Parameter passed into the task. */
                3,                          /* Priority at which the task is created. */
                NULL );                     /* Used to pass out the created task's handle. */

//    xTaskCreate(lcd_data_task,               /* Function that implements the task. */
//                "LCD",                  /* Text name for the task. */
//                configMINIMAL_STACK_SIZE,                        /* Stack size in words, not bytes. */
//                NULL,                       /* Parameter passed into the task. */
//                1,                          /* Priority at which the task is created. */
//                NULL );                     /* Used to pass out the created task's handle. */
//
//    xTaskCreate(lcd_cmd_task,               /* Function that implements the task. */
//                "LCD",                  /* Text name for the task. */
//                configMINIMAL_STACK_SIZE,                        /* Stack size in words, not bytes. */
//                NULL,                       /* Parameter passed into the task. */
//                1,                          /* Priority at which the task is created. */
//                NULL );                     /* Used to pass out the created task's handle. */
//
//    xTaskCreate(lcd_task,               /* Function that implements the task. */
//                "LCD",                  /* Text name for the task. */
//                configMINIMAL_STACK_SIZE,                        /* Stack size in words, not bytes. */
//                NULL,                       /* Parameter passed into the task. */
//                1,                          /* Priority at which the task is created. */
//                NULL );                     /* Used to pass out the created task's handle. */

    xTaskCreate(usb_task,               /* Function that implements the task. */
                "USB",                  /* Text name for the task. */
                configMINIMAL_STACK_SIZE,                        /* Stack size in words, not bytes. */
                NULL,                       /* Parameter passed into the task. */
                1,                          /* Priority at which the task is created. */
                NULL );                     /* Used to pass out the created task's handle. */

    ///////////////////////////////////////////////////////
    //               Start Scheduler
    ///////////////////////////////////////////////////////
    vTaskStartScheduler();
}


