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
#include "cluster/cl_idma_hal.h"

void vApplicationMallocFailedHook(void);
void vApplicationIdleHook(void);
void vApplicationStackOverflowHook(TaskHandle_t pxTask, char *pcTaskName);
void vApplicationTickHook(void);

#define MAX_BUFFER_SIZE 0x400

// Allocation on L2/L1 at compilation time
#define L2_DATA __attribute__((section(".l2_data")))
#define L1_DATA __attribute__((section(".data_l1")))

L2_DATA static unsigned char ext[MAX_BUFFER_SIZE * 8];
L1_DATA static unsigned char loc[MAX_BUFFER_SIZE * 8];

#define EXT_DATA_ADDR  ((unsigned int)ext)
#define TCDM_DATA_ADDR ((unsigned int)loc)


int test_idma(unsigned int dst_addr, unsigned int src_addr,
	      unsigned int num_bytes)
{

	int error = 0;
	// Initialize data
	for (int i = 0; i < num_bytes; i++) {
		*(volatile unsigned char *)(src_addr + i) = i & 0xFF;
	}

	for (int i = 0; i < num_bytes + 16; i++) {
		*(volatile unsigned char *)(dst_addr + i) = 0;
	}
	// DMA data copy
	unsigned int dma_id = pulp_cl_idma_memcpy(dst_addr, src_addr, num_bytes);

	plp_cl_dma_wait(dma_id);

	// DMA data copy check
	unsigned int test, read;

	for (int i = 0; i < num_bytes; i++) {
		test = i & 0xFF;
		read = *(volatile unsigned char *)(dst_addr + i);
		if (test != read) {
			hal_eu_mutex_lock(0);
			printf("Error!!! Read: %x, Test:%x, Index: %d \n ",
			       read, test, i);
			hal_eu_mutex_unlock(0);
			error++;
		}
	}

	for (int i = num_bytes; i < num_bytes + 16; i++) {

		test = 0;
		read = *(volatile unsigned char *)(dst_addr + i);

		if (test != read) {
			hal_eu_mutex_lock(0);
			printf("Error!!! Read: %x, Test:%x, Index: %d \n ",
			       read, test, i);
			hal_eu_mutex_unlock(0);
			error++;
		}
	}

	return error;
}


/*
 * This is the entry point executed by all processing elements when the cluster
 * controller is doing a fork
 */

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

	// Use cluster idma to move some data between L2 and L1
	// Cluster core 0 programs the idma to perform a data transfer

	int error = 0;

	// Test the transfers with different number of bytes
	for (int i = 1; i < MAX_BUFFER_SIZE; i *= 2) {

		hal_eu_mutex_lock(0);
		printf("Starting test for %d transfers from %p to %p\n", i,
		       EXT_DATA_ADDR, TCDM_DATA_ADDR);
		hal_eu_mutex_unlock(0);

		error += test_idma(TCDM_DATA_ADDR, EXT_DATA_ADDR, i);
	}

	if (error == 0) {
		hal_eu_mutex_lock(0);
		printf("Test succeeded.\n");
		hal_eu_mutex_unlock(0);
		exit(0);
	} else {
		hal_eu_mutex_lock(0);
		printf("Test failed.\n");
		hal_eu_mutex_unlock(0);
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
