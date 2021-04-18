/*
 * buzzer.h
 *
 *  Created on: Aug 6, 2020
 *      Author: steph
 */

#ifndef BUZZER_H_
#define BUZZER_H_

#include <stdint.h>

void init_buzzer_lcdbl(void);

void set_buzzer_period(uint32_t freq);

void stop_buzzer(void);

void set_lcdbl_duty_period(uint32_t duty_period);

#endif /* BUZZER_H_ */
