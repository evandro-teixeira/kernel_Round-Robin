/*
 * kernel.c
 *
 *  Created on: 29 de abr de 2018
 *      Author: evandro
 */

#include "../inc/kernel.h"


static KernelStr m_task_table;
static KernelState m_state = KERNEL_STATE_DEFAULT;
volatile TaskStr *kernel_curr_task;
volatile TaskStr *kernel_next_task;


bool Kernel_Init(void)
{
	if (m_state != KERNEL_STATE_DEFAULT)
		return false;

	memset(&m_task_table, 0, sizeof(m_task_table));
	m_state = KERNEL_STATE_INITIALIZED;

	return true;
}

bool Kernel_Add_Task(void (*handler)(void *p_params), void *p_task_params,uint32_t size)
{
	if (m_state != KERNEL_STATE_INITIALIZED && m_state != KERNEL_STATE_TASKS_INITIALIZED)
		return false;

	if (m_task_table.size >= KERNEL_CONFIG_MAX_TASKS-1)
		return false;

	uint32_t *stack_size;

	stack_size 					= malloc(size * sizeof(uint32_t));
	uint32_t stack_offset 		= (size * sizeof(uint32_t));
	TaskStr *p_task 			= &m_task_table.tasks[m_task_table.size];
	p_task->handler 			= handler;
	p_task->p_params 			= p_task_params;
	p_task->sp 					= (uint32_t)(stack_size+stack_offset-16);
	p_task->status 				= KERNEL_TASK_STATUS_IDLE;
	stack_size[stack_offset-1] 	= 0x1000000;
	stack_size[stack_offset-2] 	= (uint32_t)handler;
	stack_size[stack_offset-8] 	= (uint32_t)p_task_params;

#if KERNEL_CONFIG_DEBUG
	uint32_t base = (m_task_table.size+1)*1000;
	p_stack[stack_offset-4] = base+12;  /* R12 */
	p_stack[stack_offset-5] = base+3;   /* R3  */
	p_stack[stack_offset-6] = base+2;   /* R2  */
	p_stack[stack_offset-7] = base+1;   /* R1  */
	/* p_stack[stack_offset-8] is R0 */
	p_stack[stack_offset-9] = base+7;   /* R7  */
	p_stack[stack_offset-10] = base+6;  /* R6  */
	p_stack[stack_offset-11] = base+5;  /* R5  */
	p_stack[stack_offset-12] = base+4;  /* R4  */
	p_stack[stack_offset-13] = base+11; /* R11 */
	p_stack[stack_offset-14] = base+10; /* R10 */
	p_stack[stack_offset-15] = base+9;  /* R9  */
	p_stack[stack_offset-16] = base+8;  /* R8  */
#endif /* KERNEL_CONFIG_DEBUG */

	m_state = KERNEL_STATE_TASKS_INITIALIZED;
	m_task_table.size++;

	return true;
}

bool Kernel_Start(uint32_t systick_ticks)
{
	if (m_state != KERNEL_STATE_TASKS_INITIALIZED)
		return false;

	NVIC_SetPriority(PendSV_IRQn, 0xff);
	NVIC_SetPriority(SysTick_IRQn, 0x00);

	SysTick_Config(systick_ticks/1000);

	kernel_curr_task = &m_task_table.tasks[m_task_table.current_task];
	m_state = KERNEL_STATE_STARTED;

	__set_PSP(kernel_curr_task->sp+64);
	__set_CONTROL(0x03);
	__ISB();

	kernel_curr_task->handler(kernel_curr_task->p_params);

	return true;
}


void Kernel_Systick_Callback(void)
{
	kernel_curr_task = &m_task_table.tasks[m_task_table.current_task];
	kernel_curr_task->status = KERNEL_TASK_STATUS_IDLE;

	// Select next task:
	m_task_table.current_task++;
	if (m_task_table.current_task >= m_task_table.size)
		m_task_table.current_task = 0;

	kernel_next_task = &m_task_table.tasks[m_task_table.current_task];
	kernel_next_task->status = KERNEL_TASK_STATUS_ACTIVE;

	// Trigger PendSV which performs the actual context switch:
	SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
}

/*
void Kernel_PendSV_Callback(void)
{
	// Disable interrupts:
	__asm volatile (
	//__asm (
	//".syntax unified 			\n"
	//"cpu cortex-m0  			\n"
	//".fpu softvfp    			\n"
	//".thumb					\n"
	"cpsid	i					\n"
	"mrs	r0, psp				\n"
	//"subs	r0, #16				\n"
	"SUB	r0, #16				\n"
	"stmia	r0!,{r4-r7}			\n"
	"mov	r4, r8				\n"
	"mov	r5, r9				\n"
	"mov	r6, r10				\n"
	"mov	r7, r11				\n"
	//"subs	r0, #32				\n"
	"SUB	r0, #32				\n"
	"stmia	r0!,{r4-r7}			\n"
	//"subs	r0, #16				\n"
	"SUB	r0, #16				\n"
	// Save current task's SP:
	"ldr	r2, =os_curr_task	\n"
	"ldr	r1, [r2]			\n"
	"str	r0, [r1]			\n"
	// Load next task's SP:
	"ldr	r2, =os_next_task	\n"
	"ldr	r1, [r2]			\n"
	"ldr	r0, [r1]			\n"
	"ldmia	r0!,{r4-r7}			\n"
	"mov	r8, r4				\n"
	"mov	r9, r5				\n"
	"mov	r10, r6				\n"
	"mov	r11, r7				\n"
	"ldmia	r0!,{r4-r7}			\n"
	"msr	psp, r0				\n"
	// EXC_RETURN - Thread mode with PSP:
	"ldr r0, =0xFFFFFFFD		\n"
	// Enable interrupts:
	"cpsie	i					\n"
	"bx	r0						\n");
}
*/

//void PendSV_Handler(void)
//{
//	Kernel_PendSV_Callback();
//}
