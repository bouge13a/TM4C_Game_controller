/*
 * buzzer.c
 *
 *  Created on: Aug 6, 2020
 *      Author: steph
 */

#include <stdbool.h>

#include "buzzer_lcdbl.h"

#include "driverlib/inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/pwm.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"

void init_buzzer_lcdbl(void) {

    // Enable the GPIO port that is used for the PWM output.
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

    // The PWM peripheral must be enabled for use.
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM1);

    SysCtlGPIOAHBEnable(SYSCTL_PERIPH_GPIOF);

    // Configure the PWM function for this pin.
    MAP_GPIOPinConfigure(GPIO_PF2_M1PWM6);
    MAP_GPIOPinConfigure(GPIO_PF3_M1PWM7);
    MAP_GPIOPinTypePWM(GPIO_PORTF_AHB_BASE, GPIO_PIN_2);
    MAP_GPIOPinTypePWM(GPIO_PORTF_AHB_BASE, GPIO_PIN_3);

    // Configure PWM0 to count down without synchronization.
    MAP_PWMGenConfigure(PWM1_BASE, PWM_GEN_3,
                        PWM_GEN_MODE_DOWN | PWM_GEN_MODE_NO_SYNC);

    // Set the PWM period to 1Hz.  To calculate the appropriate parameter
    // use the following equation: N = (1 / f) * SysClk.  Where N is the
    // function parameter, f is the desired frequency, and SysClk is the
    // system clock frequency.
    MAP_PWMGenPeriodSet(PWM1_BASE, PWM_GEN_3, (SysCtlClockGet() / 1));

    // Set the initial PWM1 Pulse Width to half duty cycle
    MAP_PWMPulseWidthSet(PWM1_BASE,
                         PWM_OUT_6,
                         (SysCtlClockGet() / 1)/2);
    // Set the initial PWM1 Pulse Width to half duty cycle
    MAP_PWMPulseWidthSet(PWM1_BASE,
                         PWM_OUT_7,
                         (SysCtlClockGet() / 1)/2);

    // Enable the PWM Out Bit 0 (PB6) output signal.
    MAP_PWMOutputState(PWM1_BASE, PWM_OUT_6_BIT, true);
    MAP_PWMOutputState(PWM1_BASE, PWM_OUT_7_BIT, true);

    // Enable the PWM generator block.
    MAP_PWMGenEnable(PWM1_BASE, PWM_GEN_3);

} // End init_buzzer

void set_buzzer_period(uint32_t freq) {

    MAP_PWMGenPeriodSet(PWM1_BASE, PWM_GEN_3, (SysCtlClockGet() / freq));


    // Set the initial PWM0 Pulse Width to half duty cycle
    MAP_PWMPulseWidthSet(PWM1_BASE,
                         PWM_OUT_6,
                         PWMGenPeriodGet(PWM1_BASE, PWM_GEN_3)/2);
    MAP_PWMPulseWidthSet(PWM1_BASE,
                         PWM_OUT_7,
                         PWMGenPeriodGet(PWM1_BASE, PWM_GEN_3)/2);

} // End set_frequency

void stop_buzzer(void) {
    // disable the PWM Out Bit 0 (PB6) output signal.
    MAP_PWMOutputState(PWM1_BASE, PWM_OUT_6_BIT, false);
} // End stop_buzzer

void set_lcdbl_duty_period(uint32_t duty_period) {

    MAP_PWMPulseWidthSet(PWM1_BASE,
                         PWM_OUT_7,
                         duty_period);

} // End set_lcdbl_duty_period
