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
 * Test if we can write to spi using pmsis
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
#include <stdint.h>

/* system includes */
#include "system_pmsis.h"
#include "timer_irq.h"
#include "fll.h"
#include "irq.h"
#include "gpio.h"

/* pmsis */
/* #include "pmsis.h" */

#if !defined(SYNC_CS_AUTO) && !defined(ASYNC_CS_AUTO) &&                       \
	!defined(SYNC_CS_KEEP) && !defined(ASYNC_CS_KEEP)
#define SYNC_CS_AUTO 1
#endif

#define TOTAL_SIZE (8192)
//#define TOTAL_SIZE (8192*8 + 256)
#define TOTAL_SIZE_RTL (1024)

#define NB_BUFFERS 4

void vApplicationMallocFailedHook(void);
void vApplicationStackOverflowHook(TaskHandle_t pxTask, char *pcTaskName);

static inline void get_info(int *buffer_size)
{
#if !defined(PI_PLATFORM_RTL)
	*buffer_size = TOTAL_SIZE;
#else
	if (pi_platform() == PI_PLATFORM_RTL) {
		*buffer_size = TOTAL_SIZE_RTL;
	} else {
		*buffer_size = TOTAL_SIZE;
	}
#endif
}
#undef PI_L2
#define PI_L2

PI_L2 int32_t cmd_buffer[4][2];
PI_L2 int32_t rx_cmd_buffer[4][2];

PI_L2 uint8_t *tx_buffer;

PI_L2 uint8_t *rx_buffer;

static pi_task_t buf_event[NB_BUFFERS];
static pi_task_t rx_buf_event[NB_BUFFERS];
static pi_task_t cmd_event[NB_BUFFERS];
static pi_task_t rx_cmd_event[NB_BUFFERS];

static void set_spim_verif_command(struct pi_device *spim, int cmd, int addr,
				   int size, int32_t *cmd_buffer,
				   pi_task_t *task)
{
	cmd_buffer[0] = (cmd << 24) | (size * 8);
	cmd_buffer[1] = addr;
	if (task)
		pi_spi_send_async(spim, cmd_buffer, 8 * 8, PI_SPI_CS_AUTO,
				  task);
	else
		pi_spi_send(spim, cmd_buffer, 8 * 8, PI_SPI_CS_AUTO);
}

static int test_entry()
{
	struct pi_spi_conf conf;
	struct pi_device spim;

	int total_size;
	get_info(&total_size);
	int buffer_size = total_size / NB_BUFFERS;

	pi_spi_conf_init(&conf);

	conf.wordsize = PI_SPI_WORDSIZE_32;
	conf.big_endian = 1;
	conf.max_baudrate = 10000000;
	conf.polarity = 0;
	conf.phase = 0;
	conf.itf = SPIM_ITF;
	conf.cs = SPIM_CS;

	pi_open_from_conf(&spim, &conf);

	if (pi_spi_open(&spim))
		return -1;

	printf("malloc tx buffer\n");
	tx_buffer = pmsis_l2_malloc(total_size);
	if (tx_buffer == NULL)
		return -1;

	printf("malloc rx buffer\n");
	rx_buffer = pmsis_l2_malloc(total_size);
	if (rx_buffer == NULL)
		return -1;

	printf("tx buffer init\n");
	for (int i = 0; i < total_size; i++) {
		tx_buffer[i] = i;
	}

#if defined(SYNC_CS_AUTO)

	printf("sync cs auto\n");
	for (int i = 0; i < NB_BUFFERS; i++) {
		set_spim_verif_command(&spim, 0x1, buffer_size * i, buffer_size,
				       cmd_buffer[i], NULL);
		pi_spi_send(&spim, tx_buffer + buffer_size * i, buffer_size * 8,
			    PI_SPI_CS_AUTO);

		set_spim_verif_command(&spim, 0x2, buffer_size * i, buffer_size,
				       cmd_buffer[i], NULL);
		pi_spi_receive(&spim, rx_buffer + buffer_size * i,
			       buffer_size * 8, PI_SPI_CS_AUTO);
	}

#elif defined(ASYNC_CS_AUTO)

	printf("async cs auto\n");
	for (int i = 0; i < NB_BUFFERS; i++) {
		set_spim_verif_command(&spim, 0x1, buffer_size * i, buffer_size,
				       cmd_buffer[i],
				       pi_task_block(&cmd_event[i]));
		pi_spi_send_async(&spim, tx_buffer + buffer_size * i,
				  buffer_size * 8, PI_SPI_CS_AUTO,
				  pi_task_block(&buf_event[i]));
	}

	for (int i = 0; i < NB_BUFFERS; i++) {
		set_spim_verif_command(&spim, 0x2, buffer_size * i, buffer_size,
				       rx_cmd_buffer[i],
				       pi_task_block(&rx_cmd_event[i]));
		pi_spi_receive_async(&spim, rx_buffer + buffer_size * i,
				     buffer_size * 8, PI_SPI_CS_AUTO,
				     pi_task_block(&rx_buf_event[i]));
	}

	for (int i = 0; i < NB_BUFFERS; i++) {
		pi_task_wait_on(&cmd_event[i]);
		pi_task_wait_on(&buf_event[i]);
		pi_task_wait_on(&rx_cmd_event[i]);
		pi_task_wait_on(&rx_buf_event[i]);
	}

#elif defined(SYNC_CS_KEEP)

	printf("sync cs keep\n");
	set_spim_verif_command(&spim, 0x1, 0, total_size, cmd_buffer[0], NULL);

	for (int i = 0; i < NB_BUFFERS; i++) {
		int mode =
			i == NB_BUFFERS - 1 ? PI_SPI_CS_AUTO : PI_SPI_CS_KEEP;
		pi_spi_send(&spim, tx_buffer + buffer_size * i, buffer_size * 8,
			    mode);
	}

	set_spim_verif_command(&spim, 0x2, 0, total_size, cmd_buffer[0], NULL);

	for (int i = 0; i < NB_BUFFERS; i++) {
		int mode =
			i == NB_BUFFERS - 1 ? PI_SPI_CS_AUTO : PI_SPI_CS_KEEP;
		pi_spi_receive(&spim, rx_buffer + buffer_size * i,
			       buffer_size * 8, mode);
	}

#elif defined(ASYNC_CS_KEEP)

	printf("async cs keep\n");
	set_spim_verif_command(&spim, 0x1, 0, total_size, cmd_buffer[0], NULL);

	for (int i = 0; i < NB_BUFFERS; i++) {
		int mode =
			i == NB_BUFFERS - 1 ? PI_SPI_CS_AUTO : PI_SPI_CS_KEEP;
		pi_spi_send_async(&spim, tx_buffer + buffer_size * i,
				  buffer_size * 8, mode,
				  pi_task_block(&buf_event[i]));
	}

	set_spim_verif_command(&spim, 0x2, 0, total_size, cmd_buffer[0],
			       pi_task_block(&cmd_event[0]));

	for (int i = 0; i < NB_BUFFERS; i++) {
		int mode =
			i == NB_BUFFERS - 1 ? PI_SPI_CS_AUTO : PI_SPI_CS_KEEP;
		pi_spi_receive_async(&spim, rx_buffer + buffer_size * i,
				     buffer_size * 8, mode,
				     pi_task_block(&rx_buf_event[i]));
	}

	pi_task_wait_on(&cmd_event[0]);

	for (int i = 0; i < NB_BUFFERS; i++) {
		pi_task_wait_on(&buf_event[i]);
		pi_task_wait_on(&rx_buf_event[i]);
	}

#endif

	printf("starting error check\n");
	int error = 0;
	for (int i = 0; i < total_size; i++) {
		if (rx_buffer[i] != tx_buffer[i]) {
			if (error == 0)
				printf("First error at index %d, expected 0x%x, got 0x%x at %p\n",
				       i, tx_buffer[i], rx_buffer[i],
				       &rx_buffer[i]);
			error++;
			return -1;
		}
	}

	if (error) {
		printf("Got %d errors\n", error);
	} else {
		printf("Test success\n");
	}
	pi_spi_close(&spim);
	return error;
}

static void test_kickoff(void *arg)
{
	int ret = test_entry();
	pmsis_exit(ret);
}

/* Program Entry. */
int main(void)
{
	/* Init board hardware. */
	system_init();

	printf("\n\n\t *** FreeRTOS Hello World *** \n\n");
	return pmsis_kickoff((void *)test_kickoff);
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

