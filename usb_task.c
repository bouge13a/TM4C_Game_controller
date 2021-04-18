/*
 * usb_task.c
 *
 *  Created on: Aug 9, 2020
 *      Author: steph
 */

#include "usb_task.h"

#include "FreeRTOS.h"
#include "task.h"

#include <stdbool.h>
#include <stdint.h>
#include "driverlib/inc/hw_memmap.h"
#include "driverlib/inc/hw_types.h"
#include "driverlib/inc/hw_gpio.h"
#include "driverlib/inc/hw_sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
#include "usb/usbdhidgamepad.h"
#include "usb_structs.h"
#include "buttons.h"
#include "ADC_task.h"
#include "board_pin_defs.h"
#include "logger.h"

static gpio_pin_t* a_button;
static gpio_pin_t* b_button;
static gpio_pin_t* start_button;
static gpio_pin_t* select_button;
static gpio_pin_t* js_button;

static adc_pin_t* accel_x;
static adc_pin_t* accel_y;
static adc_pin_t* accel_z;
static adc_pin_t* js_x;
static adc_pin_t* js_y;

// The the number of milliseconds to wait between usb reports
static const uint32_t REPORT_PERIOD_MS = 5;

// The HID gamepad report that is returned to the host.
static tGamepadReport sReport;

// This enumeration holds the various states that the gamepad can be in during
// normal operation.
static volatile enum {

    // Not yet configured.
    eStateNotConfigured,

    // Connected and not waiting on data to be sent.
    eStateIdle,

    // Suspended.
    eStateSuspend,

    // Connected and waiting on data to be sent out.
    eStateSending
} g_iGamepadState;

static error_t* usb_reconnected_err;
static error_t* usb_suspended_err;

// Handles asynchronous events from the HID gamepad driver.
//
// \param pvCBData is the event callback pointer provided during
// USBDHIDGamepadInit().  This is a pointer to our gamepad device structure
// (&g_sGamepadDevice).
// \param ui32Event identifies the event we are being called back for.
// \param ui32MsgData is an event-specific value.
// \param pvMsgData is an event-specific pointer.
//
// This function is called by the HID gamepad driver to inform the application
// of particular asynchronous events related to operation of the gamepad HID
// device.
//
// \return Returns 0 in all cases.
uint32_t GamepadHandler(void *pvCBData,
                        uint32_t ui32Event,
                        uint32_t ui32MsgData,
                        void *pvMsgData) {

    switch (ui32Event)
    {
        // The host has connected to us and configured the device.
        case USB_EVENT_CONNECTED: {

            g_iGamepadState = eStateIdle;

            break;
        }

        // The host has disconnected from us.
        case USB_EVENT_DISCONNECTED: {

            g_iGamepadState = eStateNotConfigured;

            set_error(usb_reconnected_err);

            break;
        }


        // This event occurs every time the host acknowledges transmission
        // of a report.  It is to return to the idle state so that a new report
        // can be sent to the host.
        case USB_EVENT_TX_COMPLETE: {

            // Enter the idle state since we finished sending something.
            g_iGamepadState = eStateIdle;

            break;
        }

        // This event indicates that the host has suspended the USB bus.
        case USB_EVENT_SUSPEND: {

            // Go to the suspended state.
            g_iGamepadState = eStateSuspend;

            set_error(usb_suspended_err);

            break;
        }

        // This event signals that the host has resumed signaling on the bus.
        case USB_EVENT_RESUME: {

            // Go back to the idle state.
            g_iGamepadState = eStateIdle;

            break;
        }

        // Return the pointer to the current report.  This call is
        // rarely if ever made, but is required by the USB HID
        // specification.
        case USBD_HID_EVENT_GET_REPORT: {

            *(void **)pvMsgData = (void *)&sReport;

            break;
        }

        // We ignore all other events.
        default: {
            break;
        }
    }

    return(0);
}

static void process_controls(void) {

    sReport.ui8Buttons = (uint8_t)(read_button(a_button) |
                                  (read_button(b_button) << 1) |
                                  (read_button(start_button) << 2) |
                                  (read_button(select_button) << 3) |
                                  (read_button(js_button) << 4));

    sReport.i8XPos = (uint8_t)(get_adc_val(accel_x) >> 4);
    sReport.i8YPos = (uint8_t)(get_adc_val(accel_y) >> 4);
    sReport.i8ZPos = (uint8_t)(get_adc_val(accel_z) >> 4);
    sReport.i8JsX  = (uint8_t)(get_adc_val(js_x) >> 4);
    sReport.i8JsY  = (uint8_t)(get_adc_val(js_y) >> 4);

} // End process_controls

void init_usb(void) {

    // Not configured initially.
    g_iGamepadState = eStateNotConfigured;

    // Enable the GPIO peripheral used for USB, and configure the USB
    // pins.
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);

    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOD));

    SysCtlGPIOAHBEnable(SYSCTL_PERIPH_GPIOD);
    MAP_GPIOPinTypeUSBAnalog(GPIO_PORTD_AHB_BASE, GPIO_PIN_4 | GPIO_PIN_5);

    // Set the USB stack mode to Device mode.
    USBStackModeSet(0, eUSBModeForceDevice, 0);

    // Pass the device information to the USB library and place the device
    // on the bus.
    USBDHIDGamepadInit(0, &g_sGamepadDevice);

    sReport.ui8Buttons = 0;
    sReport.i8XPos = 0;
    sReport.i8YPos = 0;
    sReport.i8ZPos = 0;
    sReport.i8JsX = 0;
    sReport.i8JsY = 0;

    a_button      = get_button_config("Button A");
    b_button      = get_button_config("Button B");
    select_button = get_button_config("Select");
    start_button  = get_button_config("Start");
    js_button     = get_button_config("Joystick OK");
    accel_x       = get_adc_config("accel x");
    accel_y       = get_adc_config("accel y");
    accel_z       = get_adc_config("accel z");
    js_x          = get_adc_config("joystick x");
    js_y          = get_adc_config("joystick y");

    usb_reconnected_err = create_error("USB",
                                       "USB has reconnected");

    usb_suspended_err = create_error("USB",
                                     "Host has suspended bus");

} // End init_usb

void usb_task(void* parm) {

    vTaskDelay(100);

    while(1) {

        // Wait for state to be idle
        if(g_iGamepadState == eStateIdle) {

            // Fill out sReport
            process_controls();

            // Send sReport
            USBDHIDGamepadSendReport(&g_sGamepadDevice,
                                     &sReport,
                                     sizeof(sReport));

            IntMasterDisable();
            g_iGamepadState = eStateSending;
            IntMasterEnable();
        }

        vTaskDelay(REPORT_PERIOD_MS);
    }
} // End usb_task
