/*
Amazon FreeRTOS
Copyright (C) 2018 Amazon.com, Inc. or its affiliates.  All Rights Reserved.

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

 http://aws.amazon.com/freertos
 http://www.FreeRTOS.org
*/

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"

/* System init includes. */
#include "iot_system_init.h"

/* Logging includes. */
#include "iot_logging_task.h"

#define mainLOGGING_TASK_STACK_SIZE         ( configMINIMAL_STACK_SIZE * 6 )
#define mainLOGGING_MESSAGE_QUEUE_LENGTH    ( 15 )

/**
 * @brief Application task startup hook.
 */
void vApplicationDaemonTaskStartupHook( void );

/**
 * @brief Initializes the board.
 */
static void prvMiscInitialization( void );
/*-----------------------------------------------------------*/

/**
 * @brief The application entry point from a power on reset is PowerON_Reset_PC()
 * in resetprg.c.
 */
void main( void )
{
	vTaskDelete(NULL);
}
/*-----------------------------------------------------------*/

static void prvMiscInitialization( void )
{
    /* Initialize UART for serial terminal. */
    uart_config();

    /* Start logging task. */
    xLoggingTaskInitialize( mainLOGGING_TASK_STACK_SIZE,
                            configMAX_PRIORITIES-1,
                            mainLOGGING_MESSAGE_QUEUE_LENGTH );
}
/*-----------------------------------------------------------*/

#if defined(__RXV2) // Use SWINT2 for Renesas RXv2
#include "platform.h"

extern void *test_interrupt_handler;

R_PRAGMA_INTERRUPT(tm_interrupt_handler, VECT(ICU,SWINT2))
R_ATTRIB_INTERRUPT void tm_interrupt_handler(void)
{
	((void (*)(void)) test_interrupt_handler)();
}

void tm_interrupt_raise(void)
{
	__int_exception(VECT(ICU,SWINT2));
}
#endif

#define ENABLE_PRINTF
#define printf vLoggingPrintf

#define main tm_basic_processing_test_main
#include "tm_basic_processing_test.c"
#undef main

#define main tm_cooperative_scheduling_test_main
#include "tm_cooperative_scheduling_test.c"
#undef main

#define main tm_interrupt_preemption_processing_test_main
#include "tm_interrupt_preemption_processing_test.c"
#undef main

#define main tm_interrupt_processing_test_main
#include "tm_interrupt_processing_test.c"
#undef main

#define main tm_memory_allocation_test_main
#include "tm_memory_allocation_test.c"
#undef main

#define main tm_message_processing_test_main
#include "tm_message_processing_test.c"
#undef main

#define main tm_preemptive_scheduling_test_main
#include "tm_preemptive_scheduling_test.c"
#undef main

#define main tm_synchronization_processing_test_main
#include "tm_synchronization_processing_test.c"
#undef main

void vApplicationDaemonTaskStartupHook( void )
{
    prvMiscInitialization();

    if( SYSTEM_Init() == pdPASS )
    {
    	printf("Thread-Metric Test Suite starts.\n");

    	char c = '0';
    	switch (c) {
    	case '1':
    		printf("START: Basic Processing Test\n");
    		tm_basic_processing_test_main();
    		break;

    	case '2':
    		printf("START: Cooperative Scheduling Test\n");
    		tm_cooperative_scheduling_test_main();
    		break;

    	case '3':
    		printf("START: Preemptive Scheduling Test\n");
    		tm_preemptive_scheduling_test_main();
    		break;

    	case '4':
    		printf("START: Interrupt Processing Test\n");
    		test_interrupt_handler = tm_interrupt_processing_handler;
    		tm_interrupt_processing_test_main();
    		break;

    	case '5':
    		printf("START: Interrupt Preemption Processing Test\n");
    		test_interrupt_handler = tm_interrupt_preemption_handler;
    		tm_interrupt_preemption_processing_test_main();
    		break;

    	case '6':
    		printf("START: Message Processing Test\n");
    		tm_message_processing_test_main();
    		break;

    	case '7':
    		printf("START: Synchronization Processing Test\n");
    		tm_synchronization_processing_test_main();
    		break;

    	case '8':
    		printf("START: Memory Allocation Test\n");
    		tm_memory_allocation_test_main();
    		break;

    	default:
    		printf("Unknown command: '%c'.\n", c);
    	}

    }
}

/*-----------------------------------------------------------*/
