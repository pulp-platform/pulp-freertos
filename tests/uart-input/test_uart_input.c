/*
 * Copyright 2020 Greenwaves Technologies
 * Copyright 2020 ETH Zurich
 * Copyright 2022 Georgi Bakalski aka. Irdroid
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
 *	   Georgi Bakalski	 
 */

/* FreeRTOS kernel includes. */
#include <FreeRTOS.h>
#include "task.h"

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

/* pmsis */
#include "target.h"
#include "pmsis_task.h"
#include "os.h"
#include "uart.h"
#include "implementation_specific_defines.h"


/* Variables used. */
#define BUFFER_SIZE      ( 10 )

char l2_in[BUFFER_SIZE];
struct pi_device uart;
volatile uint8_t done = 0;
char *sentence = "Type your name(10 letters) :\n";

void test_uart_input(void)
{
    struct pi_uart_conf conf;

    /* Init & open uart. */
    pi_uart_conf_init(&conf);
    conf.enable_tx = 1;
    conf.enable_rx = 1;
    conf.baudrate_bps = 115200;
    pi_open_from_conf(&uart, &conf);
    if (pi_uart_open(&uart))
    {
        printf("Uart open failed !\n");
        pmsis_exit(-1);
    }

    #if defined(UART_FLOW_CONTROL_EMU)
    {
        pi_uart_ioctl(&uart, PI_UART_IOCTL_ENABLE_FLOW_CONTROL, NULL);
        printf("Flow control enable\n");
    }
    #endif  /* UART_FLOW_CONTROL_EMU */

    /* Write on uart then wait for data from uart. */

	pi_task_t wait_tx_task = { 0 };
        pi_task_block(&wait_tx_task);
        pi_uart_write_async(&uart, sentence, strlen(sentence), &wait_tx_task);
        pi_task_wait_on(&wait_tx_task);
	
	pi_task_t wait_rx_task = { 0 };
        pi_task_block(&wait_rx_task);
        pi_uart_read_async(&uart, l2_in, BUFFER_SIZE, &wait_rx_task);
        pi_task_wait_on(&wait_rx_task);
	unsigned char l2 = l2_in[0];
	printf("Characters in L2_IN Buffer:\n");
	printf("---------------------------\n");
	for(int i=0;i<BUFFER_SIZE;i++){
	printf("%c\n",l2_in[i]);
	}
	//pi_task_t wait_tx_task1 = { 0 };
        //pi_task_block(&wait_tx_task1);

	pi_uart_write(&uart, l2_in, sizeof(l2_in));
	//pi_task_wait_on(&wait_tx_task1);

        pi_uart_close(&uart);

        pmsis_exit(0);
}

/* Program Entry. */
int main(void)
{
    system_init();	
    printf("\n\n\t *** PMSIS Uart Input Test ***\n\n");
    return pmsis_kickoff((void *) test_uart_input);
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
