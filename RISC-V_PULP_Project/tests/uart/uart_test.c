/*
* SPDX-License-Identifier: Apache-2.0
* Author: Greenwaves-Technologies
*         Robert Balas (balasr@iis.ee.ethz.ch)
*/

/*
 * Test if we can write to uart using pmsis
 */

/* FreeRTOS kernel includes. */
#include <FreeRTOS.h>
#include <task.h>

/* c stdlib */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <inttypes.h>

/* system includes */
#include "system_pulpissimo_ri5cy.h"
#include "timer_irq.h"
#include "fll.h"
#include "irq.h"
#include "gpio.h"

/* pmsis */
#include "pmsis.h"

void vApplicationMallocFailedHook(void);
void vApplicationStackOverflowHook(TaskHandle_t pxTask, char *pcTaskName);

char msg[40];

void write_uart(void)
{
	printf("Entering main controller\n");

	uint32_t errors = 0;
	struct pi_device uart;
	struct pi_uart_conf conf;

	/* Init & open uart. */
	pi_uart_conf_init(&conf);
	conf.enable_tx = 1;
	conf.enable_rx = 0;
	conf.baudrate_bps = 115200;

	pi_open_from_conf(&uart, &conf);
	if (pi_uart_open(&uart)) {
		printf("UART open failed !\n");
		pmsis_exit(-1);
	}

	uint32_t core_id = pi_core_id(), cluster_id = pi_cluster_id();

	sprintf(msg, "[%" PRIu32 " %" PRIu32 "] Hello World!\n", cluster_id,
		core_id);

	/* Write though uart. */
#if (ASYNC)
	pi_task_t wait_task = { 0 };
	pi_task_block(&wait_task);
	pi_uart_write_async(&uart, msg, strlen(msg), &wait_task);
	pi_task_wait_on(&wait_task);
#else
	pi_uart_write(&uart, msg, strlen(msg));
#endif
	pi_uart_close(&uart);

	pmsis_exit(EXIT_SUCCESS);
}

/* Program Entry. */
int main(void)
{
	/* Init board hardware. */
	system_init();

	printf("\n\n\t *** FreeRTOS Hello World *** \n\n");
	return pmsis_kickoff((void *)write_uart);
}

void vApplicationMallocFailedHook(void)
{
	/* vApplicationMallocFailedHook() will only be called if
	configUSE_MALLOC_FAILED_HOOK is set to 1 in FreeRTOSConfig.h.  It is a hook
	function that will get called if a call to pvPortMalloc() fails.
	pvPortMalloc() is called internally by the kernel whenever a task, queue,
	timer or semaphore is created.  It is also called by various parts of the
	demo application.  If heap_1.c or heap_2.c are used, then the size of the
	heap available to pvPortMalloc() is defined by configTOTAL_HEAP_SIZE in
	FreeRTOSConfig.h, and the xPortGetFreeHeapSize() API function can be used
	to query the size of free heap space that remains (although it does not
	provide information on how the remaining heap might be fragmented). */
	taskDISABLE_INTERRUPTS();
	printf("error: application malloc failed\n");
	__asm volatile("ebreak");
	for (;;)
		;
}

void vApplicationStackOverflowHook(TaskHandle_t pxTask, char *pcTaskName)
{
	(void)pcTaskName;
	(void)pxTask;

	/* Run time stack overflow checking is performed if
	configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
	function is called if a stack overflow is detected. */
	taskDISABLE_INTERRUPTS();
	printf("error: stack overflow\n");
	__asm volatile("ebreak");
	for (;;)
		;
}

void vPortSetupTimerInterrupt(void)
{
	extern int timer_irq_init(uint32_t ticks);

	/* No CLINT so use the PULP timer to generate the tick interrupt. */
	/* TODO: configKERNEL_INTERRUPT_PRIORITY - 1 ? */
	timer_irq_init(ARCHI_REF_CLOCK / configTICK_RATE_HZ);
	/* TODO: allow setting interrupt priority (to super high(?)) */
	irq_enable(IRQ_FC_EVT_TIMER0_LO);
}

void vSystemIrqHandler(uint32_t mcause)
{
	extern void (*isr_table[32])(void);
	isr_table[mcause & 0x1f]();
}
