/*
 * light_temp_task.h
 *
 *  Created on: Aug 7, 2020
 *      Author: steph
 */

#ifndef LIGHT_TEMP_TASK_H_
#define LIGHT_TEMP_TASK_H_

#include <stdint.h>

void init_light_temp(void);

void light_temp_task(void* parm);

uint32_t get_light_value(void);

void light_temp_drawpage(void);
void light_temp_drawdata(void);
void light_temp_drawinput(int character);

#endif /* LIGHT_TEMP_TASK_H_ */
