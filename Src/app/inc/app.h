/*
 * app.h
 *
 *  Created on: 29 de abr de 2018
 *      Author: evandro
 */

#ifndef APP_INC_APP_H_
#define APP_INC_APP_H_

#include <stdio.h>
#include <stdbool.h>
#include "main.h"
#include "stm32f0xx_hal.h"
#include "gpio.h"
#include "main.h"


#define TIME_LED_BLUE	200000
#define TIME_LED_GREEN	500000

#define SIZE_TASK_LED_GREEN		(uint32_t)(128*2)
#define SIZE_TASK_LED_BLUE		(uint32_t)(128*2)
#define SIZE_TASK_PUSH_BUTTON	(uint32_t)(128*2)

void app_task_led_green(void *parameters);
void app_task_led_blue(void *parameters);
void app_task_push_button(void *parameters);

#endif /* APP_INC_APP_H_ */
