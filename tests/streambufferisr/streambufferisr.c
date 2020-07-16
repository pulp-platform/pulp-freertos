/*
 * FreeRTOS Kernel V10.2.1
 * Copyright (C) 2019 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 *
 * 1 tab == 8 spaces!
 */

/* Author: Robert Balas (balasr@iis.ee.ethz.ch )*/

/* Description: This is a fork of StreamBufferDemo.c from Demo/Common. This is the
 * description from it */

/*
 * A simple example that shows a stream buffer being used to pass data from an
 * interrupt to a task.
 *
 * There are two strings, pcStringToSend and pcStringToReceive, where
 * pcStringToReceive is a substring of pcStringToSend.  The interrupt sends
 * a few bytes of pcStringToSend to a stream buffer ever few times that it
 * executes.  A task reads the bytes from the stream buffer, looking for the
 * substring, and flagging an error if the received data is invalid.
 */

/* FreeRTOS kernel includes. */
#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>
#include <stream_buffer.h>

/* c stdlib */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

/* PULPissimo includes. */
#include "system_pulpissimo_ri5cy.h"
#include "timer_irq.h"
#include "fll.h"
#include "irq.h"
#include "gpio.h"

void vApplicationMallocFailedHook(void);
void vApplicationIdleHook(void);
void vApplicationStackOverflowHook(TaskHandle_t pxTask, char *pcTaskName);
void vApplicationTickHook(void);

/* Prepare haredware to run the demo. */
static void prvSetupHardware(void);

/* hook printf */
#ifdef USE_STDIO
#define vPrintDisplayMessage(str) vSendString(*str)
#else
#define vPrintDisplayMessage(str)
#endif

/* streambufferisr declarations */
void vStartStreamBufferInterruptDemo(void);
void vBasicStreamBufferSendFromISR(void);
BaseType_t xIsInterruptStreamBufferDemoStillRunning(void);

#define STREAM_ERROR_STACK_SIZE	   ((unsigned short)configMINIMAL_STACK_SIZE)
#define STREAM_ERROR_TASK_PRIORITY (configMAX_PRIORITIES - 1)

#define STREAM_ERROR_CHECK_TICKS 20
#define STREAM_ERROR_CHECK_MS	 pdMS_TO_TICKS(STREAM_ERROR_CHECK_TICKS)
/* #define STREAM_CHECK_ITERATIONS 3 */ /* defined in Makefile */

#define sbiSTREAM_BUFFER_LENGTH_BYTES	  ((size_t)100)
#define sbiSTREAM_BUFFER_TRIGGER_LEVEL_10 ((BaseType_t)10)

/* Implements the task that receives a stream of bytes from the interrupt. */
static void prvReceivingTask(void *pvParameters);

/* The stream buffer that is used to send data from an interrupt to the task. */
static StreamBufferHandle_t xStreamBuffer = NULL;

/* The string that is sent from the interrupt to the task four bytes at a
time.  Must be multiple of 4 bytes long as the ISR sends 4 bytes at a time*/
#define STREAM_TEST_STRING "_____Hello FreeRTOS_____"
static const char *pcStringToSend = STREAM_TEST_STRING;
/* should be is 6 * 4 bytes + null termination */
static_assert(sizeof(STREAM_TEST_STRING) == 24 + 1,
	      "pcStringToSend: unexpected string length");

/* stream isr configuration */
#define STREAM_CALLS_BETWEEN_SENDS 1
#define STREAM_BYTES_TO_SEND	   4
static const BaseType_t xCallsBetweenSends = STREAM_CALLS_BETWEEN_SENDS;
static const BaseType_t xBytesToSend = STREAM_BYTES_TO_SEND;
static_assert((sizeof(STREAM_TEST_STRING) - 1) % STREAM_BYTES_TO_SEND == 0,
	      "string length is not multiple of xBytesToSend");

/* Every (xCallsBetweenSends + 1) the global tick sends data to the stream buffer.
 * We have to transmit the whole string before the error checker tasks
 * interrupts and checks for the correct result i.e. STREAM_ERROR_CHECK_TICKS needs
 * to large enough */
static_assert((STREAM_ERROR_CHECK_TICKS / (STREAM_CALLS_BETWEEN_SENDS + 1)) *
			      STREAM_BYTES_TO_SEND >=
		      (sizeof(STREAM_TEST_STRING) - 1),
	      "error tasks check period is too small");

/* The string to task is looking for, which must be a substring of
pcStringToSend. */
static const char *pcStringToReceive = "Hello FreeRTOS";

/* Set to pdFAIL if anything unexpected happens. */
static BaseType_t xDemoStatus = pdPASS;

/* Incremented each time pcStringToReceive is correctly received, provided no
errors have occurred.  Used so the check task can check this task is still
running as expected. */
static uint32_t ulCycleCount = 0;

/* The check task, as described at the top of this file. */
static void prvCheckTask(void *pvParameters);

/* Send a message to the UART initialised in prvSetupHardware. */
void vSendString(const char *const pcString);

int main(void)
{
	prvSetupHardware();

	/* initialize stream buffer receive task */
	vStartStreamBufferInterruptDemo();

	/* Create the task that performs the 'check' functionality. */
	printf("xTaskCreate prvCheckTask (polls for errors)\n");
	xTaskCreate(prvCheckTask, "Check", STREAM_ERROR_STACK_SIZE, NULL,
		    STREAM_ERROR_TASK_PRIORITY, NULL);

	/* Start the tasks and timer running. */
	printf("Start Scheduler\n");
	vTaskStartScheduler();

	/* scheduler loops forever, we should never reach this */
	exit(EXIT_FAILURE);
}

static void prvSetupHardware(void)
{
	/* Init board hardware. */
	pulp_sys_init();
}

void vSendString(const char *const pcString)
{
	/* TODO: UART dumping */
	printf("%s", pcString);
}

void vStartStreamBufferInterruptDemo(void)
{
	/* Create the stream buffer that sends data from the interrupt to the
	task, and create the task. */
	xStreamBuffer =
		xStreamBufferCreate(/* The buffer length in bytes. */
				    sbiSTREAM_BUFFER_LENGTH_BYTES,
				    /* The stream buffer's trigger level. */
				    sbiSTREAM_BUFFER_TRIGGER_LEVEL_10);

	xTaskCreate(
		prvReceivingTask, /* The function that implements the task. */
		"StrIntRx", /* Human readable name for the task. */
		configMINIMAL_STACK_SIZE, /* Stack size (in words!). */
		NULL, /* Task parameter is not used. */
		tskIDLE_PRIORITY +
			2, /* The priority at which the task is created. */
		NULL); /* No use for the task handle. */
}

static void prvReceivingTask(void *pvParameters)
{
	char cRxBuffer[20];
	BaseType_t xNextByte = 0;

	/* Remove warning about unused parameters. */
	(void)pvParameters;

	/* Make sure the string will fit in the Rx buffer, including the NULL
	terminator. */
	configASSERT(sizeof(cRxBuffer) > strlen(pcStringToReceive));

	/* Make sure the stream buffer has been created. */
	configASSERT(xStreamBuffer != NULL);

	/* Start with the Rx buffer in a known state. */
	memset(cRxBuffer, 0x00, sizeof(cRxBuffer));

	for (;;) {
		/* Keep receiving characters until the end of the string is received.
		Note:  An infinite block time is used to simplify the example.  Infinite
		block times are not recommended in production code as they do not allow
		for error recovery. */
		xStreamBufferReceive(/* The stream buffer data is being received from. */
				     xStreamBuffer,
				     /* Where to place received data. */
				     (void *)&(cRxBuffer[xNextByte]),
				     /* The number of bytes to receive. */
				     sizeof(char),
				     /* The time to wait for the next data if the buffer
							  is empty. */
				     portMAX_DELAY);

		/* If xNextByte is 0 then this task is looking for the start of the
		string, which is 'H'. */
		if (xNextByte == 0) {
			if (cRxBuffer[xNextByte] == 'H') {
				/* The start of the string has been found.  Now receive
				characters until the end of the string is found. */
				xNextByte++;
			}
		} else {
			/* Receiving characters while looking for the end of the string,
			which is an 'S'. */
			if (cRxBuffer[xNextByte] == 'S') {
				/* The string has now been received.  Check its validity. */
				if (strcmp(cRxBuffer, pcStringToReceive) != 0) {
					xDemoStatus = pdFAIL;
				}

				/* Return to start looking for the beginning of the string
				again. */
				memset(cRxBuffer, 0x00, sizeof(cRxBuffer));
				xNextByte = 0;

				/* Increment the cycle count as an indication to the check task
				that this demo is still running. */
				if (xDemoStatus == pdPASS) {
					ulCycleCount++;
				}
			} else {
				/* Receive the next character the next time around, while
				continuing to look for the end of the string. */
				xNextByte++;

				configASSERT((size_t)xNextByte <
					     sizeof(cRxBuffer));
			}
		}
	}
}

void vBasicStreamBufferSendFromISR(void)
{
	static size_t xNextByteToSend = 0;
	/* we set those above in the declarations */
	/* const BaseType_t xCallsBetweenSends = 100, xBytesToSend = 4; */
	static BaseType_t xCallCount = 0;

	/* Is it time to write to the stream buffer again? */
	xCallCount++;
	if (xCallCount > xCallsBetweenSends) {
		xCallCount = 0;

		/* Send the next four bytes to the stream buffer. */
		xStreamBufferSendFromISR(
			xStreamBuffer,
			(const void *)(pcStringToSend + xNextByteToSend),
			xBytesToSend, NULL);

		/* Send the next four bytes the next time around, wrapping to the start
		of the string if necessary. */
		xNextByteToSend += xBytesToSend;

		if (xNextByteToSend >= strlen(pcStringToSend)) {
			xNextByteToSend = 0;
		}
	}
}

BaseType_t xIsInterruptStreamBufferDemoStillRunning(void)
{
	uint32_t ulLastCycleCount = 0;

	/* Check the demo is still running. */
	if (ulLastCycleCount == ulCycleCount) {
		xDemoStatus = pdFAIL;
	} else {
		ulLastCycleCount = ulCycleCount;
	}

	return xDemoStatus;
}

/* periodically check if the queues work and finish after a fixed amount of iterations */
static void prvCheckTask(void *pvParameters)
{
	const char *pcPassMsg = "Pass\n";
	const char *pcFailMsg =
		"ERROR: stream buffer isr test is probably not running correctly\n";

	TickType_t xDelayPeriod = STREAM_ERROR_CHECK_MS;
	TickType_t xLastExecutionTime;

	/* Initialise xLastExecutionTime so the first call to vTaskDelayUntil()
	works correctly. */
	xLastExecutionTime = xTaskGetTickCount();

	for (int i = 0; i < STREAM_CHECK_ITERATIONS; i++) {
		vTaskDelayUntil(&xLastExecutionTime, xDelayPeriod);

		if (xIsInterruptStreamBufferDemoStillRunning() == pdFALSE) {
			vPrintDisplayMessage(&pcFailMsg);
			exit(EXIT_FAILURE); /* note: normally tasks never exit */
		}

		vPrintDisplayMessage(&pcPassMsg);
	}

	/* Since tasks normally never exit, this only works on simulators or
	 * when you poll the exit status register via jtag periodically to
	 * emulate this behavior
	 */
	exit(EXIT_SUCCESS);
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

void vApplicationIdleHook(void)
{
	/* vApplicationIdleHook() will only be called if configUSE_IDLE_HOOK is set
	to 1 in FreeRTOSConfig.h.  It will be called on each iteration of the idle
	task.  It is essential that code added to this hook function never attempts
	to block in any way (for example, call xQueueReceive() with a block time
	specified, or call vTaskDelay()).  If the application makes use of the
	vTaskDelete() API function (as this demo application does) then it is also
	important that vApplicationIdleHook() is permitted to return to its calling
	function, because it is the responsibility of the idle task to clean up
	memory allocated by the kernel to any task that has since been deleted. */
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
	/* periodically trigger */
	vBasicStreamBufferSendFromISR();
}

