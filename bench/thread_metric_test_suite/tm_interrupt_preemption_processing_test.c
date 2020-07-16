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
/**   Interrupt Preemption Processing Test                                */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/

#include "tm_api.h"
//#include <stdio.h>


/* Define the counters used in the demo application...  */

volatile unsigned long   tm_interrupt_preemption_thread_0_counter;
volatile unsigned long   tm_interrupt_preemption_thread_1_counter;
volatile unsigned long   tm_interrupt_preemption_handler_counter;


/* Define the test thread prototypes.  */

void            tm_interrupt_preemption_thread_0_entry(void);
void            tm_interrupt_preemption_thread_1_entry(void);
void            tm_interrupt_preemption_handler_entry(void);


/* Define the reporting thread prototype.  */

void            tm_interrupt_preemption_thread_report(void);


/* Define the interrupt handler.  This must be called from the RTOS.  */

void            tm_interrupt_preemption_handler(void);


/* Define the initialization prototype.  */

void            tm_interrupt_preemption_processing_initialize(void);


/* Define main entry point.  */

int main()
{

    /* Initialize the test.  */
    tm_initialize(tm_interrupt_preemption_processing_initialize);
    return 0;
}


/* Define the interrupt processing test initialization.  */

void  tm_interrupt_preemption_processing_initialize(void)
{

    /* Create interrupt thread at priority 3.  */
    tm_thread_create(0, 3, tm_interrupt_preemption_thread_0_entry);

    /* Create a semaphore that will be posted from the interrupt
       handler.  */
    tm_semaphore_create(0);

    /* Resume just thread 0.  */
    tm_thread_resume(0);

    /* Create thread that generates the interrupt at priority 10.  */
    tm_thread_create(1, 10, tm_interrupt_preemption_thread_1_entry);

    /* Resume just thread 1.  */
    tm_thread_resume(1);

    /* Create the reporting thread. It will preempt the other 
       threads and print out the test results.  */
    tm_thread_create(5, 2, tm_interrupt_preemption_thread_report);
    tm_thread_resume(5);

}


/* Define the interrupt thread.  This thread is resumed from the 
   interrupt handler.  It runs and suspends.  */
void  tm_interrupt_preemption_thread_0_entry(void)
{
	int status;


	    /* Pickup the semaphore since it is initialized to 1 by default. */
	    status = tm_semaphore_get(0);

	    /* Check for good status.  */
	    if (status != TM_SUCCESS)
	        return;

    while(1)
    {

        /* Increment this thread's counter.  */
        tm_interrupt_preemption_thread_0_counter++;

        /* Wait the semaphore set by the interrupt handler. */
        status = tm_semaphore_wait(0);

        /* Check for good status.  */
        if (status != TM_SUCCESS)
            return;
    }
}

/* Define the thread that generates the interrupt.  */
void  tm_interrupt_preemption_thread_1_entry(void)
{

    while(1)
    {

        /* Force an interrupt. The underlying RTOS must see that the 
           the interrupt handler is called from the appropriate software
           interrupt or trap. */
        tm_interrupt_raise();
      
        /* We won't get back here until the interrupt processing is complete,
           including the execution of the higher priority thread made ready
           by the interrupt.  */

        /* Increment this thread's counter.  */
        tm_interrupt_preemption_thread_1_counter++;
    }
}


/* Define the interrupt handler.  This must be called from the RTOS trap handler.
   To be fair, it must behave just like a processor interrupt, i.e. it must save
   the full context of the interrupted thread during the preemption processing. */
   
void  tm_interrupt_preemption_handler(void)
{

    tm_interrupt_preemption_handler_counter++;      /* Increment the interrupt count.  */

    /* Put the semaphore from the interrupt handler.  */
    tm_semaphore_put_from_isr(0);
}


/* Define the interrupt test reporting thread.  */
void  tm_interrupt_preemption_thread_report(void)
{

unsigned long   total;
unsigned long   relative_time;
unsigned long   last_total;
unsigned long   average;


    /* Initialize the last total.  */
    last_total =  0;

    /* Initialize the relative time.  */
    relative_time =  0;

    while(1)
    {

        /* Sleep to allow the test to run.  */
        tm_thread_sleep(TM_TEST_DURATION);

        /* Increment the relative time.  */
        relative_time =  relative_time + TM_TEST_DURATION;
#ifdef ENABLE_PRINTF
        /* Print results to the stdio window.  */
        printf("**** Thread-Metric Interrupt Preemption Processing Test **** Relative Time: %lu\n", relative_time);
#endif
        /* Calculate the total of all the counters.  */
        total =  tm_interrupt_preemption_thread_0_counter + tm_interrupt_preemption_thread_1_counter + tm_interrupt_preemption_handler_counter;

        /* Calculate the average of all the counters.  */
        average =  total/3;

        /* See if there are any errors.  */
        if ((tm_interrupt_preemption_thread_0_counter < (average - 1)) || 
            (tm_interrupt_preemption_thread_0_counter > (average + 1)) ||
            (tm_interrupt_preemption_thread_1_counter < (average - 1)) || 
            (tm_interrupt_preemption_thread_1_counter > (average + 1)) ||
            (tm_interrupt_preemption_handler_counter < (average - 1)) || 
            (tm_interrupt_preemption_handler_counter > (average + 1)))
        {
#ifdef ENABLE_PRINTF
            printf("ERROR: Invalid counter value(s). Interrupt processing test has failed!\n");
#else
            while(1)
            {

            }
#endif
        }
#ifdef ENABLE_PRINTF
        /* Show the total interrupts for the time period.  */
        printf("Time Period Total:  %lu\n\n", tm_interrupt_preemption_handler_counter - last_total);
#endif
        /* Save the last total number of interrupts.  */
        last_total =  tm_interrupt_preemption_handler_counter;
    }
}
