/*
 * Copyright 2020 ETH Zurich
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * SPDX-License-Identifier: Apache-2.0
 * Author: Robert Balas (balasr@iis.ee.ethz.ch)
 */

/*
 * A minimal pmsis example. Mainly used to test if we can link against pmsis.
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
#include "system.h"
#include "timer.h"
#include "timer_irq.h"
#include "fll.h"
#include "irq.h"
#include "gpio.h"

/* pmsis */
#include "target.h"
#include "os.h"

void vApplicationMallocFailedHook(void);
void vApplicationIdleHook(void);
void vApplicationStackOverflowHook(TaskHandle_t pxTask, char *pcTaskName);
void vApplicationTickHook(void);


static int blink_times;

void timer1_handler(void)
{
	printf("blink\n");
	/* terminate after a few blinks */
	blink_times++;
	if (blink_times > 4)
		exit(0);
}

/* some busy freertos task that doesn't do anything */
void some_busy_task(void)
{
	for (;;)
		;
}

/* Program Entry. */
int main(void)
{
	/* create a dummy task */
	BaseType_t xTask;
	xTask = xTaskCreate(some_busy_task, "dummy",
			    ((unsigned short)configMINIMAL_STACK_SIZE), NULL,
			    tskIDLE_PRIORITY + 1, NULL);
	if (xTask != pdPASS) {
		printf("failed to create task\n");
		exit(1);
	}

	/* hook up timer1 interrupt */
	irq_set_handler(FC_TIMER1_IRQN, timer1_handler);

	/* reset timer (not really necessary in this case) */
	writew(1, (uintptr_t)(PULP_FC_TIMER_ADDR + TIMER_RESET_HI_OFFSET));

	/* set interrupt frequency to TIMER1_TICK_RATE_HZ */
#define TIMER1_TICK_RATE_HZ ((TickType_t)2000)
	writew(ARCHI_REF_CLOCK / TIMER1_TICK_RATE_HZ,
	       (uintptr_t)(PULP_FC_TIMER_ADDR + TIMER_CMP_HI_OFFSET));

	/* Enable timer (TIMER_CFG_HI_ENABLE_MASK), use 32khz ref clock as
	 * source (TIMER_CFG_HI_CLKCFG_MASK). Timer will reset automatically
	 * (TIMER_CFG_HI_MODE_MASK) to zero after causing an interrupt
	 * (TIMER_CFG_HI_IRQEN_MASK). Also reset timer to start from a clean
	 * slate (TIMER_CFG_HI_RESET_MASK).
	 */
	writew(TIMER_CFG_HI_ENABLE_MASK | TIMER_CFG_HI_RESET_MASK |
		       TIMER_CFG_HI_CLKCFG_MASK | TIMER_CFG_HI_MODE_MASK |
		       TIMER_CFG_HI_IRQEN_MASK,
	       (uintptr_t)(PULP_FC_TIMER_ADDR + TIMER_CFG_HI_OFFSET));

	/* Enable timer1 interrupt. Need to enable this in the CV32E40P and the
	 * apb_interrupt controller. In RI5CY we didn't need to touch the clint
	 * since it was in a kind of "passthrough" mode. */
	irq_enable(IRQ_FC_EVT_TIMER0_HI);
	irq_clint_enable(IRQ_FC_EVT_TIMER0_HI);

	/* start test */
	printf("Starting timer test\n");

	/* start scheduling */
	vTaskStartScheduler();

	/* should never happen */
	for (;;)
		;
}


void vApplicationStackOverflowHook(TaskHandle_t pxTask, char *pcTaskName)
{
	(void)pcTaskName;
	(void)pxTask;

	taskDISABLE_INTERRUPTS();
	printf("error: stack overflow\n");
	__asm volatile("ebreak");
	for (;;)
		;
}
