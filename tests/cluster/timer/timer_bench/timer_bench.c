/*
 * Copyright (C) 2017 ETH Zurich, University of Bologna and GreenWaves
 * Technologies All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 * Authors: Germain Haugou, ETH (germain.haugou@iis.ee.ethz.ch)
 */

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
#include "io.h"
#include "timer.h"
#include "timer_hal.h"
#include "timer_irq.h"
#include "fll.h"
#include "irq.h"
#include "gpio.h"

/* pmsis */
#include "cluster/fc_to_cl_delegate.h"
#include "cluster/event_unit.h"
#include "device.h"
#include "target.h"
#include "os.h"

void vApplicationMallocFailedHook(void);
void vApplicationIdleHook(void);
void vApplicationStackOverflowHook(TaskHandle_t pxTask, char *pcTaskName);
void vApplicationTickHook(void);


/* This is the entry point executed by the cluster controller */
static void cluster_entry(void *arg)
{
	/* Currently, the printing is a bit messed up, because printf shares
	 * some internal buffers among the cores which get clobbered. The
	 * correct thing to do here is to a acquire a lock before printf(). */
	hal_eu_mutex_lock(0);
	printf("(%ld, %ld) Entering cluster controller\n", pi_cluster_id(),
	       pi_core_id());
	hal_eu_mutex_unlock(0);

        // Setup FC timer 0, LO
        timer_id_t timer0_id = TIMER_LO_ID;
        // Setup FC timer 1, HI
        timer_id_t timer1_id = TIMER_HI_ID;
        
        reset_timer_cl(timer0_id);
        reset_timer_cl(timer1_id);
        
        start_timer_cl(timer0_id);
        start_timer_cl(timer1_id);
        
        for (int i=0; i<100; i++) {
        	
        }
        
        stop_timer_cl(timer0_id);
        stop_timer_cl(timer1_id);
        
        uint32_t time0_elapsed = get_timer_cnt_cl(timer0_id);
        uint32_t time1_elapsed = get_timer_cnt_cl(timer1_id);
        
	hal_eu_mutex_lock(0);	
        printf("Timer 0 elapsed: %d\nTimer 1 elapsed: %d\n", time0_elapsed, time1_elapsed);
	hal_eu_mutex_unlock(0);

	if (time0_elapsed == 0 || time1_elapsed == 0) {
	    exit(1);
	}
}


int test_entry()
{
	printf("(%ld, %ld) Entering main controller\n", pi_cluster_id(),
	       pi_core_id());

	struct pi_device cluster_dev;
	struct pi_cluster_conf conf;
	struct pi_cluster_task cluster_task;
	struct pi_task task;

	/* First open the cluster */
	pi_cluster_conf_init(&conf);

	pi_open_from_conf(&cluster_dev, &conf);

	if (pi_cluster_open(&cluster_dev))
		return -1;

	/* Then offload an entry point, this will get executed on the cluster
	 * controller */
	pi_cluster_send_task_to_cl(
		&cluster_dev,
		pi_cluster_task(&cluster_task, cluster_entry, NULL));

	return 0;
}

void test_kickoff(void *arg)
{
	int ret = test_entry();
	pmsis_exit(ret);
}

int main()
{
	BaseType_t xTask;

	system_init();

	/* Disable printf output buffering to prevent cluster cores clobbering
	 * shared buffer. */
	if (setvbuf(stdout, NULL, _IONBF, 0))
		return 1;

	xTask = xTaskCreate(test_kickoff, "test_entry",
			    ((unsigned short)configMINIMAL_STACK_SIZE), NULL,
			    (configMAX_PRIORITIES - 1), NULL);
	if (xTask != pdPASS)
		exit(EXIT_FAILURE);

	vTaskStartScheduler();

	/* should never happen */
	return EXIT_FAILURE;
}


void vApplicationMallocFailedHook(void)
{
	/* vApplicationMallocFailedHook() will only be called if
	configUSE_MALLOC_FAILED_HOOK is set to 1 in FreeRTOSConfig.h.  It is a
	hook function that will get called if a call to pvPortMalloc() fails.
	pvPortMalloc() is called internally by the kernel whenever a task,
	queue, timer or semaphore is created.  It is also called by various
	parts of the demo application.  If heap_1.c or heap_2.c are used, then
	the size of the heap available to pvPortMalloc() is defined by
	configTOTAL_HEAP_SIZE in FreeRTOSConfig.h, and the
	xPortGetFreeHeapSize() API function can be used to query the size of
	free heap space that remains (although it does not provide information
	on how the remaining heap might be fragmented). */
	taskDISABLE_INTERRUPTS();
	printf("error: application malloc failed\n");
	__asm volatile("ebreak");
	for (;;)
		;
}

void vApplicationIdleHook(void)
{
	/* vApplicationIdleHook() will only be called if configUSE_IDLE_HOOK is
	set to 1 in FreeRTOSConfig.h.  It will be called on each iteration of
	the idle task.  It is essential that code added to this hook function
	never attempts to block in any way (for example, call xQueueReceive()
	with a block time specified, or call vTaskDelay()).  If the application
	makes use of the vTaskDelete() API function (as this demo application
	does) then it is also important that vApplicationIdleHook() is permitted
	to return to its calling function, because it is the responsibility of
	the idle task to clean up memory allocated by the kernel to any task
	that has since been deleted. */
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

void vApplicationTickHook(void)
{
}
