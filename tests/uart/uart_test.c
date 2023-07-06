/*
 * Copyright 2020 Greenwaves Technologies
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
 * Author: Germain Hagou
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
#include "system.h"
#include "timer_irq.h"
#include "fll.h"
#include "irq.h"
#include "gpio.h"

#include "target.h"
/* pmsis */
#include "device.h"
#include "os.h"
#include "uart.h"

void vApplicationMallocFailedHook(void);
void vApplicationStackOverflowHook(TaskHandle_t pxTask, char *pcTaskName);

char msg[40];

void write_uart(void)
{
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

	return pmsis_kickoff((void *)write_uart);
}

void vApplicationMallocFailedHook(void)
{
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

