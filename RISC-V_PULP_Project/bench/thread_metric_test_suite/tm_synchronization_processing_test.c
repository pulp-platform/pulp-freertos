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
/**   Synchronization Processing Test                                     */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/

#include "tm_api.h"
//#include "stdio.h"


/* Define the counters used in the demo application...  */

volatile unsigned long   tm_synchronization_processing_counter;


/* Define the test thread prototypes.  */

void            tm_synchronization_processing_thread_0_entry(void);


/* Define the reporting thread prototype.  */

void            tm_synchronization_processing_thread_report(void);


/* Define the initialization prototype.  */

void            tm_synchronization_processing_initialize(void);


/* Define main entry point.  */

int main()
{

    /* Initialize the test.  */
    tm_initialize(tm_synchronization_processing_initialize);
    return 0;
}


/* Define the synchronization processing test initialization.  */

void  tm_synchronization_processing_initialize(void)
{

    /* Create thread 0 at priority 10.  */
    tm_thread_create(0, 10, tm_synchronization_processing_thread_0_entry);

    /* Resume thread 0.  */
    tm_thread_resume(0);

    /* Create a semaphore for the test.  */
    tm_semaphore_create(0);

    /* Create the reporting thread. It will preempt the other 
       threads and print out the test results.  */
    tm_thread_create(5, 2, tm_synchronization_processing_thread_report);
    tm_thread_resume(5);
}


/* Define the synchronization processing thread.  */
void  tm_synchronization_processing_thread_0_entry(void)
{

int status;

    while(1)
    {

        /* Get the semaphore.  */
        tm_semaphore_get(0);

        /* Release the semaphore.  */
        status = tm_semaphore_put(0);

        /* Check for semaphore put error.  */
        if (status != TM_SUCCESS)
            break;

        /* Increment the number of semaphore get/puts.  */
        tm_synchronization_processing_counter++;
    }
}


/* Define the synchronization test reporting thread.  */
void  tm_synchronization_processing_thread_report(void)
{

unsigned long   last_counter;
unsigned long   relative_time;


    /* Initialize the last counter.  */
    last_counter =  0;

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
        printf("**** Thread-Metric Synchronization Processing Test **** Relative Time: %lu\n", relative_time);
#endif
        /* See if there are any errors.  */
        if (tm_synchronization_processing_counter == last_counter)
        {
#ifdef ENABLE_PRINTF
            printf("ERROR: Invalid counter value(s). Error getting/putting semaphore!\n");
#else
            while(1)
            {

            }
#endif
        }
#ifdef ENABLE_PRINTF
        /* Show the time period total.  */
        printf("Time Period Total:  %lu\n\n", tm_synchronization_processing_counter - last_counter);
#endif
        /* Save the last counter.  */
        last_counter =  tm_synchronization_processing_counter;
    }
}
