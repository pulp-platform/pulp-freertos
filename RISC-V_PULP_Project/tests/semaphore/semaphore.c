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

/* Description: This is a fork of semtest.c from Demo/Common. This is the
 * description from it */
/**
 * Creates two sets of two tasks.  The tasks within a set share a variable, access
 * to which is guarded by a semaphore.
 *
 * Each task starts by attempting to obtain the semaphore.  On obtaining a
 * semaphore a task checks to ensure that the guarded variable has an expected
 * value.  It then clears the variable to zero before counting it back up to the
 * expected value in increments of 1.  After each increment the variable is checked
 * to ensure it contains the value to which it was just set. When the starting
 * value is again reached the task releases the semaphore giving the other task in
 * the set a chance to do exactly the same thing.  The starting value is high
 * enough to ensure that a tick is likely to occur during the incrementing loop.
 *
 * An error is flagged if at any time during the process a shared variable is
 * found to have a value other than that expected.  Such an occurrence would
 * suggest an error in the mutual exclusion mechanism by which access to the
 * variable is restricted.
 *
 * The first set of two tasks poll their semaphore.  The second set use blocking
 * calls.
 *
 * \page semaphore semaphore.c
 * \ingroup tests
 * <HR>
 */
/* FreeRTOS kernel includes. */
#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>
#include <semphr.h>

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

/* hook printf */
#ifdef USE_STDIO
#define vPrintDisplayMessage(str) vSendString(*str)
#else
#define vPrintDisplayMessage(str)
#endif

/* The value to which the shared variables are counted. */
#define semtstBLOCKING_EXPECTED_VALUE	  ((unsigned long)0xfff)
#define semtstNON_BLOCKING_EXPECTED_VALUE ((unsigned long)0xff)

#define SEM_STACK_SIZE	     ((unsigned short)configMINIMAL_STACK_SIZE)
#define SEM_ERROR_STACK_SIZE ((unsigned short)configMINIMAL_STACK_SIZE)

#define semtstNUM_TASKS (4)

#define semtstDELAY_FACTOR ((TickType_t)10)

#define SEM_TASK_PRIORITY	(tskIDLE_PRIORITY + 1)
#define SEM_ERROR_TASK_PRIORITY (configMAX_PRIORITIES - 1)
#define SEM_ERROR_CHECK_MS	pdMS_TO_TICKS(100)

/* The task function as described at the top of the file. */
static void prvSemaphoreTest(void *pvParameters);

/* Structure used to pass parameters to each task. */
typedef struct SEMAPHORE_PARAMETERS {
	SemaphoreHandle_t xSemaphore;
	volatile unsigned long *pulSharedVariable;
	TickType_t xBlockTime;
} xSemaphoreParameters;

/* Variables used to check that all the tasks are still running without errors. */
static volatile short sCheckVariables[semtstNUM_TASKS] = { 0 };
static volatile short sNextCheckVariable = 0;

/* Strings to print if USE_STDIO is defined. */
const char *const pcPollingSemaphoreTaskError =
	"Guarded shared variable in unexpected state.\n";
const char *const pcSemaphoreTaskStart =
	"Guarded shared variable task started.\n";

/* launch semaphore tasks */
void vStartSemaphoreTasks(UBaseType_t uxPriority);

/* The check task, as described at the top of this file. */
static void prvCheckTask(void *pvParameters);

/* Send a message to the UART initialised in prvSetupHardware. */
void vSendString(const char *const pcString);

int main(void)
{
	prvSetupHardware();

	printf("Start semaphore tasks\n");
	vStartSemaphoreTasks(SEM_TASK_PRIORITY);

	/* Create the task that performs the 'check' functionality. */
	printf("xTaskCreate prvCheckTask (polls for errors)\n");
	xTaskCreate(prvCheckTask, "Check", SEM_ERROR_STACK_SIZE, NULL,
		    SEM_ERROR_TASK_PRIORITY, NULL);

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

void vStartSemaphoreTasks(UBaseType_t uxPriority)
{
	xSemaphoreParameters *pxFirstSemaphoreParameters,
		*pxSecondSemaphoreParameters;
	const TickType_t xBlockTime =
		(TickType_t)10; /*TODO: figure out value */

	/* Create the structure used to pass parameters to the first two tasks. */
	pxFirstSemaphoreParameters = (xSemaphoreParameters *)pvPortMalloc(
		sizeof(xSemaphoreParameters));

	if (pxFirstSemaphoreParameters != NULL) {
		/* Create the semaphore used by the first two tasks. */
		vSemaphoreCreateBinary(pxFirstSemaphoreParameters->xSemaphore);

		if (pxFirstSemaphoreParameters->xSemaphore != NULL) {
			/* Create the variable which is to be shared by the first two tasks. */
			pxFirstSemaphoreParameters->pulSharedVariable =
				(unsigned long *)pvPortMalloc(
					sizeof(unsigned long));

			/* Initialise the share variable to the value the tasks expect. */
			*(pxFirstSemaphoreParameters->pulSharedVariable) =
				semtstNON_BLOCKING_EXPECTED_VALUE;

			/* The first two tasks do not block on semaphore calls. */
			pxFirstSemaphoreParameters->xBlockTime = (TickType_t)0;

			/* Spawn the first two tasks.  As they poll they operate at the idle priority. */
			xTaskCreate(prvSemaphoreTest, "PolSEM1", SEM_STACK_SIZE,
				    (void *)pxFirstSemaphoreParameters,
				    tskIDLE_PRIORITY, (TaskHandle_t *)NULL);
			xTaskCreate(prvSemaphoreTest, "PolSEM2", SEM_STACK_SIZE,
				    (void *)pxFirstSemaphoreParameters,
				    tskIDLE_PRIORITY, (TaskHandle_t *)NULL);
		}
	}

	/* Do exactly the same to create the second set of tasks, only this time
	provide a block time for the semaphore calls. */
	pxSecondSemaphoreParameters = (xSemaphoreParameters *)pvPortMalloc(
		sizeof(xSemaphoreParameters));
	if (pxSecondSemaphoreParameters != NULL) {
		vSemaphoreCreateBinary(pxSecondSemaphoreParameters->xSemaphore);

		if (pxSecondSemaphoreParameters->xSemaphore != NULL) {
			pxSecondSemaphoreParameters->pulSharedVariable =
				(unsigned long *)pvPortMalloc(
					sizeof(unsigned long));
			*(pxSecondSemaphoreParameters->pulSharedVariable) =
				semtstBLOCKING_EXPECTED_VALUE;
			pxSecondSemaphoreParameters->xBlockTime =
				xBlockTime / portTICK_PERIOD_MS;

			xTaskCreate(prvSemaphoreTest, "BlkSEM1", SEM_STACK_SIZE,
				    (void *)pxSecondSemaphoreParameters,
				    uxPriority, (TaskHandle_t *)NULL);
			xTaskCreate(prvSemaphoreTest, "BlkSEM2", SEM_STACK_SIZE,
				    (void *)pxSecondSemaphoreParameters,
				    uxPriority, (TaskHandle_t *)NULL);
		}
	}
}

static void prvSemaphoreTest(void *pvParameters)
{
	xSemaphoreParameters *pxParameters;
	volatile unsigned long *pulSharedVariable, ulExpectedValue;
	unsigned long ulCounter;
	short sError = pdFALSE, sCheckVariableToUse;

	/* See which check variable to use.  sNextCheckVariable is not semaphore
	protected! */
	portENTER_CRITICAL();
	sCheckVariableToUse = sNextCheckVariable;
	sNextCheckVariable++;
	portEXIT_CRITICAL();

	/* Queue a message for printing to say the task has started. */
	vPrintDisplayMessage(&pcSemaphoreTaskStart);

	/* A structure is passed in as the parameter.  This contains the shared
	variable being guarded. */
	pxParameters = (xSemaphoreParameters *)pvParameters;
	pulSharedVariable = pxParameters->pulSharedVariable;

	/* If we are blocking we use a much higher count to ensure loads of context
	switches occur during the count. */
	if (pxParameters->xBlockTime > (TickType_t)0) {
		ulExpectedValue = semtstBLOCKING_EXPECTED_VALUE;
	} else {
		ulExpectedValue = semtstNON_BLOCKING_EXPECTED_VALUE;
	}

	for (;;) {
		/* Try to obtain the semaphore. */
		if (xSemaphoreTake(pxParameters->xSemaphore,
				   pxParameters->xBlockTime) == pdPASS) {
			/* We have the semaphore and so expect any other tasks using the
			shared variable to have left it in the state we expect to find
			it. */
			if (*pulSharedVariable != ulExpectedValue) {
				vPrintDisplayMessage(
					&pcPollingSemaphoreTaskError);
				sError = pdTRUE;
			}

			/* Clear the variable, then count it back up to the expected value
			before releasing the semaphore.  Would expect a context switch or
			two during this time. */
			for (ulCounter = (unsigned long)0;
			     ulCounter <= ulExpectedValue; ulCounter++) {
				*pulSharedVariable = ulCounter;
				if (*pulSharedVariable != ulCounter) {
					if (sError == pdFALSE) {
						vPrintDisplayMessage(
							&pcPollingSemaphoreTaskError);
					}
					sError = pdTRUE;
				}
			}

			/* Release the semaphore, and if no errors have occurred increment the check
			variable. */
			if (xSemaphoreGive(pxParameters->xSemaphore) ==
			    pdFALSE) {
				vPrintDisplayMessage(
					&pcPollingSemaphoreTaskError);
				sError = pdTRUE;
			}

			if (sError == pdFALSE) {
				if (sCheckVariableToUse < semtstNUM_TASKS) {
					(sCheckVariables[sCheckVariableToUse])++;
				}
			}

			/* If we have a block time then we are running at a priority higher
			than the idle priority.  This task takes a long time to complete
			a cycle	(deliberately so to test the guarding) so will be starving
			out lower priority tasks.  Block for some time to allow give lower
			priority tasks some processor time. */
			vTaskDelay(pxParameters->xBlockTime *
				   semtstDELAY_FACTOR);
		} else {
			if (pxParameters->xBlockTime == (TickType_t)0) {
				/* We have not got the semaphore yet, so no point using the
				processor.  We are not blocking when attempting to obtain the
				semaphore. */
				taskYIELD();
			}
		}
	}
}

/* This is called to check that all the created tasks are still running with no errors. */
BaseType_t xAreSemaphoreTasksStillRunning(void)
{
	static short sLastCheckVariables[semtstNUM_TASKS] = { 0 };
	BaseType_t xTask, xReturn = pdTRUE;

	for (xTask = 0; xTask < semtstNUM_TASKS; xTask++) {
		if (sLastCheckVariables[xTask] == sCheckVariables[xTask]) {
			xReturn = pdFALSE;
		}

		sLastCheckVariables[xTask] = sCheckVariables[xTask];
	}

	return xReturn;
}

/* periodically check if the queues work and finish after a fixed amount of iterations */
static void prvCheckTask(void *pvParameters)
{
	const char *pcPassMsg = "Pass\n";
	const char *pcFailMsg =
		"ERROR: Semaphore test is probably not running correctly\n";

	TickType_t xDelayPeriod = SEM_ERROR_CHECK_MS;
	TickType_t xLastExecutionTime;

	/* Initialise xLastExecutionTime so the first call to vTaskDelayUntil()
	works correctly. */
	xLastExecutionTime = xTaskGetTickCount();

	for (int i = 0; i < SEM_CHECK_ITERATIONS; i++) {
		vTaskDelayUntil(&xLastExecutionTime, xDelayPeriod);

		if (xAreSemaphoreTasksStillRunning() == pdFALSE) {
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
