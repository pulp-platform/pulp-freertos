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

/* Description: This is a fork of PollQ.c from Demo/Common. This is the
 * description from it */
/**
 * This is a very simple queue test.  See the BlockQ. c documentation for a more
 * comprehensive version.
 *
 * Creates two tasks that communicate over a single queue.  One task acts as a
 * producer, the other a consumer.
 *
 * The producer loops for three iteration, posting an incrementing number onto the
 * queue each cycle.  It then delays for a fixed period before doing exactly the
 * same again.
 *
 * The consumer loops emptying the queue.  Each item removed from the queue is
 * checked to ensure it contains the expected value.  When the queue is empty it
 * blocks for a fixed period, then does the same again.
 *
 * All queue access is performed without blocking.  The consumer completely empties
 * the queue each time it runs so the producer should never find the queue full.
 *
 * An error is flagged if the consumer obtains an unexpected value or the producer
 * find the queue is full.
 *
 * \page PollQC pollQ.c
 * \ingroup DemoFiles
 * <HR>
 */

/* FreeRTOS kernel includes. */
#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>

/* c stdlib */
#include <stdio.h>
#include <stdlib.h>

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

#define QUEUE_STACK_SIZE       ((unsigned short)configMINIMAL_STACK_SIZE)
#define QUEUE_ERROR_STACK_SIZE ((unsigned short)configMINIMAL_STACK_SIZE)

/* hook printf */
#ifdef USE_STDIO
#define vPrintDisplayMessage(str) vSendString(*str)
#else
#define vPrintDisplayMessage(str)
#endif

#define QUEUE_TASK_PRIORITY	  (tskIDLE_PRIORITY + 1)
#define QUEUE_ERROR_TASK_PRIORITY (configMAX_PRIORITIES - 1)
#define QUEUE_PRODUCER_SEND_MS	  pdMS_TO_TICKS(10)
#define QUEUE_CONSUMER_RECV_MS	  pdMS_TO_TICKS(10)
#define QUEUE_ERROR_CHECK_MS	  pdMS_TO_TICKS(20)
/* how many iterations we want to run the error check tasks. Afterwards it will
 * terminate the program with exit */
/* #define QUEUE_CHECK_ITERATIONS 2 */ /* set via gcc flags */

/* The task that posts the incrementing number onto the queue. */
static void vPolledQueueProducer(void *pvParameters);

/* The task that empties the queue. */
static void vPolledQueueConsumer(void *pvParameters);

/* Variables that are used to check that the tasks are still running with no errors. */
static volatile short sPollingConsumerCount = 0, sPollingProducerCount = 0;

/* launch queue tasks */
void vStartPolledQueueTasks(UBaseType_t uxPriority);

/* The check task, as described at the top of this file. */
static void prvCheckTask(void *pvParameters);

/* Send a message to the UART initialised in prvSetupHardware. */
void vSendString(const char *const pcString);

int main(void)
{
	prvSetupHardware();

	printf("Start PolledQueueTasks\n");
	vStartPolledQueueTasks(QUEUE_TASK_PRIORITY);

	/* Create the task that performs the 'check' functionality. */
	printf("xTaskCreate prvCheckTask (polls for errors)\n");
	xTaskCreate(prvCheckTask, "Check", QUEUE_ERROR_STACK_SIZE, NULL,
		    QUEUE_ERROR_TASK_PRIORITY, NULL);

	printf("Start Scheduler\n");
	/* Start the tasks and timer running. */
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

void vStartPolledQueueTasks(UBaseType_t uxPriority)
{
	static QueueHandle_t xPolledQueue;
	const UBaseType_t uxQueueSize = 10;

	/* Create the queue used by the producer and consumer. */
	xPolledQueue =
		xQueueCreate(uxQueueSize, (UBaseType_t)sizeof(unsigned short));

	/* Spawn the producer and consumer. */
	xTaskCreate(vPolledQueueConsumer, "QConsNB", QUEUE_STACK_SIZE,
		    (void *)&xPolledQueue, uxPriority, NULL);
	xTaskCreate(vPolledQueueProducer, "QProdNB", QUEUE_STACK_SIZE,
		    (void *)&xPolledQueue, uxPriority, NULL);
}

static void vPolledQueueProducer(void *pvParameters)
{
	unsigned short usValue = 0, usLoop;
	QueueHandle_t *pxQueue;
	/* const TickType_t xDelay = (TickType_t)200 / portTICK_PERIOD_MS; */
	const TickType_t xDelay = QUEUE_PRODUCER_SEND_MS;
	const unsigned short usNumToProduce = 3;
	const char *const pcTaskStartMsg = "Polled queue producer started.\n";
	const char *const pcTaskErrorMsg = "Could not post on polled queue.\n";
	short sError = pdFALSE;

	/* Queue a message for printing to say the task has started. */
	vPrintDisplayMessage(&pcTaskStartMsg);

	/* The queue being used is passed in as the parameter. */
	pxQueue = (QueueHandle_t *)pvParameters;

	for (;;) {
		for (usLoop = 0; usLoop < usNumToProduce; ++usLoop) {
			/* Send an incrementing number on the queue without blocking. */
			if (xQueueSendToBack(*pxQueue, (void *)&usValue,
					     (TickType_t)0) != pdPASS) {
				/* We should never find the queue full - this is an error. */
				vPrintDisplayMessage(&pcTaskErrorMsg);
				sError = pdTRUE;
			} else {
				if (sError == pdFALSE) {
					/* If an error has ever been recorded we stop incrementing the
					check variable. */
					++sPollingProducerCount;
				}

				/* Update the value we are going to post next time around. */
				++usValue;
			}
		}

		/* Wait before we start posting again to ensure the consumer runs and
		empties the queue. */
		vTaskDelay(xDelay);
	}
}

static void vPolledQueueConsumer(void *pvParameters)
{
	unsigned short usData, usExpectedValue = 0;
	QueueHandle_t *pxQueue;
	/* const TickType_t xDelay = (TickType_t)200 / portTICK_PERIOD_MS; */
	const TickType_t xDelay = QUEUE_CONSUMER_RECV_MS;

	const char *const pcTaskStartMsg = "Polled queue consumer started.\n";
	const char *const pcTaskErrorMsg =
		"Incorrect value received on polled queue.\n";
	short sError = pdFALSE;

	/* Queue a message for printing to say the task has started. */
	vPrintDisplayMessage(&pcTaskStartMsg);

	/* The queue being used is passed in as the parameter. */
	pxQueue = (QueueHandle_t *)pvParameters;

	for (;;) {
		/* Loop until the queue is empty. */
		while (uxQueueMessagesWaiting(*pxQueue)) {
			if (xQueueReceive(*pxQueue, &usData, (TickType_t)0) ==
			    pdPASS) {
				if (usData != usExpectedValue) {
					/* This is not what we expected to receive so an error has
					occurred. */
					vPrintDisplayMessage(&pcTaskErrorMsg);
					sError = pdTRUE;
					/* Catch-up to the value we received so our next expected value
					should again be correct. */
					usExpectedValue = usData;
				} else {
					if (sError == pdFALSE) {
						/* Only increment the check variable if no errors have
						occurred. */
						++sPollingConsumerCount;
					}
				}
				++usExpectedValue;
			}
		}

		/* Now the queue is empty we block, allowing the producer to place more
		items in the queue. */
		vTaskDelay(xDelay);
	}
}

/* This is called to check that all the created tasks are still running with no errors. */
BaseType_t xArePollingQueuesStillRunning(void)
{
	static short sLastPollingConsumerCount = 0,
		     sLastPollingProducerCount = 0;
	BaseType_t xReturn;

	if ((sLastPollingConsumerCount == sPollingConsumerCount) ||
	    (sLastPollingProducerCount == sPollingProducerCount)) {
		xReturn = pdFALSE;
	} else {
		xReturn = pdTRUE;
	}

	sLastPollingConsumerCount = sPollingConsumerCount;
	sLastPollingProducerCount = sPollingProducerCount;

	return xReturn;
}

/* periodically check if the queues work and finish after a fixed amount of iterations */
static void prvCheckTask(void *pvParameters)
{
	const char *pcPassMsg = "Pass\n";
	const char *pcFailMsg =
		"ERROR: Queues are probably not running correctly\n";

	TickType_t xDelayPeriod = QUEUE_ERROR_CHECK_MS;
	TickType_t xLastExecutionTime;

	/* Initialise xLastExecutionTime so the first call to vTaskDelayUntil()
	works correctly. */
	xLastExecutionTime = xTaskGetTickCount();

	for (int i = 0; i < QUEUE_CHECK_ITERATIONS; i++) {
		vTaskDelayUntil(&xLastExecutionTime, xDelayPeriod);

		if (xArePollingQueuesStillRunning() == pdFALSE) {
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

/* At the time of writing, interrupt nesting is not supported, so do not use
the default vSystemIrqHandler() implementation as that enables interrupts.  A
version that does not enable interrupts is provided below.  THIS INTERRUPT
HANDLER IS SPECIFIC TO THE VEGA BOARD WHICH DOES NOT INCLUDE A CLINT! */
void vSystemIrqHandler(uint32_t mcause)
{
	extern void (*isr_table[32])(void);
	isr_table[mcause & 0xf]();
}
