/**************************************************************************/ 
/*                                                                        */ 
/*            Copyright (c) 1996-2016 by Express Logic Inc.               */ 
/*                                                                        */ 
/*  This Original Work may be modified, distributed, or otherwise used in */ 
/*  any manner with no obligations other than the following:              */ 
/*                                                                        */ 
/*    1. This legend must be retained in its entirety in any source code  */ 
/*       copies of this Work.                                             */ 
/*                                                                        */ 
/*    2. This software may not be used in the development of an operating */
/*       system product.                                                  */ 
/*                                                                        */  
/*  This Original Work is hereby provided on an "AS IS" BASIS and WITHOUT */ 
/*  WARRANTY, either express or implied, including, without limitation,   */ 
/*  the warranties of NON-INFRINGEMENT, MERCHANTABILITY or FITNESS FOR A  */ 
/*  PARTICULAR PURPOSE. THE ENTIRE RISK AS TO THE QUALITY OF this         */ 
/*  ORIGINAL WORK IS WITH the user.                                       */ 
/*                                                                        */ 
/*  Express Logic, Inc. reserves the right to modify this software        */ 
/*  without notice.                                                       */ 
/*                                                                        */ 
/*  Express Logic, Inc.                     info@expresslogic.com         */
/*  11423 West Bernardo Court               http://www.expresslogic.com   */
/*  San Diego, CA  92127                                                  */
/*                                                                        */
/**************************************************************************/

/**************************************************************************/
/**************************************************************************/
/**                                                                       */ 
/** Thread-Metric Component                                               */
/**                                                                       */
/**   Porting Layer (Must be completed with RTOS specifics)               */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/


/* Include necessary files.  */

#include    "tm_api.h"

/* Kernel includes */
#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>
#include <semphr.h>

/* Define FreeRTOS mapping constants. */
#define TM_FREERTOS_MAX_THREADS         10
#define TM_FREERTOS_MAX_QUEUES           1
#define TM_FREERTOS_MAX_SEMAPHORES       1

/* Define FreeRTOS data structures. */
TaskHandle_t            tm_thread_array[TM_FREERTOS_MAX_THREADS];
QueueHandle_t           tm_queue_array[TM_FREERTOS_MAX_QUEUES];
SemaphoreHandle_t       tm_semaphore_array[TM_FREERTOS_MAX_SEMAPHORES];

/* This function called from main performs basic RTOS initialization,
   calls the test initialization function, and then starts the RTOS function.  */
void  tm_initialize(void (*test_initialization_function)(void)) {
    /* Call initialization function. */
    test_initialization_function();
    /* Enter the FreeRTOS kernel. */
     //vTaskStartScheduler();
}


/* This function takes a thread ID and priority and attempts to create the
   file in the underlying RTOS.  Valid priorities range from 1 through 31, 
   where 1 is the highest priority and 31 is the lowest. If successful, 
   the function should return TM_SUCCESS. Otherwise, TM_ERROR should be returned.   */
int  tm_thread_create(int thread_id, int priority, void (*entry_function)(void))
{
    int new_priority = 10 - priority + 1;
    BaseType_t status;

//    configASSERT(new_priority <= (configMAX_PRIORITIES - 1));

    status = xTaskCreate(entry_function, "Thread-Metric test",
                        configMINIMAL_STACK_SIZE, NULL, /*priority*/new_priority,
                        &tm_thread_array[thread_id]);

    if (status != pdPASS) {
        return TM_ERROR;
    }

    /* threads start active */
//    tm_thread_suspend(thread_id);

    return TM_SUCCESS;

}


/* This function resumes the specified thread.  If successful, the function should
   return TM_SUCCESS. Otherwise, TM_ERROR should be returned.  */
int  tm_thread_resume(int thread_id)
{
    vTaskResume(tm_thread_array[thread_id]);

    return TM_SUCCESS;
}


/* This function suspends the specified thread.  If successful, the function should
   return TM_SUCCESS. Otherwise, TM_ERROR should be returned.  */
int  tm_thread_suspend(int thread_id)
{
    vTaskSuspend(tm_thread_array[thread_id]);

    return TM_SUCCESS;
}


/* This function relinquishes to other ready threads at the same
   priority.  */
void tm_thread_relinquish(void)
{
    taskYIELD();
}


/* This function suspends the specified thread for the specified number
   of seconds.  If successful, the function should return TM_SUCCESS. 
   Otherwise, TM_ERROR should be returned.  */
void tm_thread_sleep(int seconds)
{
    vTaskDelay((seconds * 1000U) / portTICK_RATE_MS);
}


/* This function creates the specified queue.  If successful, the function should
   return TM_SUCCESS. Otherwise, TM_ERROR should be returned.  */
int  tm_queue_create(int queue_id)
{
    tm_queue_array[queue_id] = xQueueCreate(10, 4 * sizeof(int32_t));

    if (tm_queue_array[queue_id] == NULL) {
        return TM_ERROR;
    }

    return TM_SUCCESS;
}


/* This function sends a 16-byte message to the specified queue.  If successful, 
   the function should return TM_SUCCESS. Otherwise, TM_ERROR should be returned.  */
int  tm_queue_send(int queue_id, unsigned long *message_ptr)
{
	BaseType_t status;

	status = xQueueSendToBack(tm_queue_array[queue_id], (const void*) message_ptr, (TickType_t) 0);

	if (status != pdTRUE) {
		return TM_ERROR;
	}

	return TM_SUCCESS;
}


/* This function receives a 16-byte message from the specified queue.  If successful, 
   the function should return TM_SUCCESS. Otherwise, TM_ERROR should be returned.  */
int  tm_queue_receive(int queue_id, unsigned long *message_ptr)
{
	BaseType_t status;

	status = xQueueReceive(tm_queue_array[queue_id], (void* const) message_ptr, (TickType_t) 0);

	if (status != pdTRUE) {
		return TM_ERROR;
	}

	return TM_SUCCESS;
}


/* This function creates the specified semaphore.  If successful, the function should
   return TM_SUCCESS. Otherwise, TM_ERROR should be returned.  */
int  tm_semaphore_create(int semaphore_id)
{
	tm_semaphore_array[semaphore_id] = xSemaphoreCreateBinary();

	if (tm_semaphore_array[semaphore_id] == NULL) {
		return TM_ERROR;
	}

	/* so it starts available */
	return tm_semaphore_put(semaphore_id);
}


/* This function gets the specified semaphore.  If successful, the function should
   return TM_SUCCESS. Otherwise, TM_ERROR should be returned.  */
int  tm_semaphore_get(int semaphore_id)
{
	BaseType_t status;

	status = xSemaphoreTake(tm_semaphore_array[semaphore_id], (TickType_t) 0);

	if (status != pdTRUE) {
		return TM_ERROR;
	}

	return TM_SUCCESS;
}


/* This function waits the specified semaphore.  If successful, the function should
   return TM_SUCCESS. Otherwise, TM_ERROR should be returned.  */
int  tm_semaphore_wait(int semaphore_id)
{
	BaseType_t status;

	status = xSemaphoreTake(tm_semaphore_array[semaphore_id], portMAX_DELAY);

	if (status != pdTRUE) {
		return TM_ERROR;
	}

	return TM_SUCCESS;
}


/* This function puts the specified semaphore.  If successful, the function should
   return TM_SUCCESS. Otherwise, TM_ERROR should be returned.  */
int  tm_semaphore_put(int semaphore_id)
{
	BaseType_t status;

	status = xSemaphoreGive(tm_semaphore_array[semaphore_id]);

	if (status != pdTRUE) {
		return TM_ERROR;
	}

	return TM_SUCCESS;
}


/* This function puts the specified semaphore.  If successful, the function should
   return TM_SUCCESS. Otherwise, TM_ERROR should be returned.  */
int  tm_semaphore_put_from_isr(int semaphore_id)
{
	BaseType_t yield_required;
	BaseType_t status;

	status = xSemaphoreGiveFromISR(tm_semaphore_array[semaphore_id], &yield_required);

	if (status != pdTRUE) {
		return TM_ERROR;
	}

	portYIELD_FROM_ISR(yield_required);

	return TM_SUCCESS;
}


/* This function creates the specified memory pool that can support one or more
   allocations of 128 bytes.  If successful, the function should
   return TM_SUCCESS. Otherwise, TM_ERROR should be returned.  */
int  tm_memory_pool_create(int pool_id)
{
	return TM_SUCCESS;
}


/* This function allocates a 128 byte block from the specified memory pool.  
   If successful, the function should return TM_SUCCESS. Otherwise, TM_ERROR 
   should be returned.  */
int  tm_memory_pool_allocate(int pool_id, unsigned char **memory_ptr)
{
	*memory_ptr = pvPortMalloc(128);

	if (*memory_ptr == NULL) {
		return TM_ERROR;
	}

	return TM_SUCCESS;
}


/* This function releases a previously allocated 128 byte block from the specified 
   memory pool. If successful, the function should return TM_SUCCESS. Otherwise, TM_ERROR 
   should be returned.  */
int  tm_memory_pool_deallocate(int pool_id, unsigned char *memory_ptr)
{
	vPortFree(memory_ptr);

	return TM_SUCCESS;
}
