/*
 * app.c
 *
 *  Created on: 29 de abr de 2018
 *      Author: evandro
 */

#include "../inc/app.h"

static bool flag_push_button = false;

static void app_flag_push_button_set(bool flag);
static bool app_flag_push_button_get(void);

static void app_flag_push_button_set(bool flag)
{
	flag_push_button = flag;
}

static bool app_flag_push_button_get(void)
{
	return flag_push_button;
}

void app_task_push_button(void *parameters)
{
	while(1)
	{
		if(HAL_GPIO_ReadPin(GPIOA,B1_Pin) == GPIO_PIN_SET)
		{
			app_flag_push_button_set(true);
		}
	}
}

void app_task_led_green(void *parameters)
{
	static uint32_t time_delay = 0;
	uint32_t i = (uint32_t)parameters;

	while(1)
	{
		time_delay++;
		if(time_delay >= i)
		{
			time_delay = 0;
			HAL_GPIO_TogglePin(GPIOC, LD3_Pin);
		}
	}
}

void app_task_led_blue(void *parameters)
{
	uint32_t i = 0;

	while(1)
	{
		if(app_flag_push_button_get() == true)
		{
			app_flag_push_button_set(false);
			HAL_GPIO_TogglePin(GPIOC, LD4_Pin);
			for(i=0;i<1000;i++);
		}
	}
}
