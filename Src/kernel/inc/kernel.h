/*
 * kernel.h
 *
 *  Created on: 29 de abr de 2018
 *      Author: evandro
 */

#ifndef KERNEL_INC_KERNEL_H_
#define KERNEL_INC_KERNEL_H_

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include "stm32f0xx_hal.h"
#include "stm32f0xx.h"

#define KERNEL_CONFIG_MAX_TASKS		8
#define KERNEL_CONFIG_DEBUG 		0

typedef enum
{
	KERNEL_TASK_STATUS_IDLE = 1,
	KERNEL_TASK_STATUS_ACTIVE,
}TaskStatus;

typedef struct
{
	volatile uint32_t sp;
	void (*handler)(void *p_params);
	void *p_params;
	volatile TaskStatus status;
}TaskStr;

typedef enum
{
	KERNEL_STATE_DEFAULT = 1,
	KERNEL_STATE_INITIALIZED,
	KERNEL_STATE_TASKS_INITIALIZED,
	KERNEL_STATE_STARTED,
}KernelState;

typedef struct
{
	TaskStr tasks[KERNEL_CONFIG_MAX_TASKS];
	volatile uint32_t current_task;
	uint32_t size;
}KernelStr;

bool Kernel_Init(void);
bool Kernel_Add_Task(void (*handler)(void *p_params), void *p_task_params,uint32_t size);
bool Kernel_Start(uint32_t systick_ticks);
void Kernel_PendSV_Callback(void);
void Kernel_Systick_Callback(void);

#endif /* KERNEL_INC_KERNEL_H_ */
