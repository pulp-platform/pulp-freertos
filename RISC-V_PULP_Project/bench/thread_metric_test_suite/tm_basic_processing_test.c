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
/**   Basic Processing Test                                               */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/

#include "tm_api.h"
//#include "stdio.h"


/* Define the counters used in the demo application...  */

volatile unsigned long   tm_basic_processing_counter;


/* Test array.  We will just do a series of calculations on the 
   test array to eat up processing bandwidth. The idea is that 
   all RTOSes should produce the same metric here if everything
   else is equal, e.g. processor speed, memory speed, etc.  */

volatile unsigned long   tm_basic_processing_array[1024];


/* Define the test thread prototypes.  */

void            tm_basic_processing_thread_0_entry(void);


/* Define the reporting thread prototype.  */

void            tm_basic_processing_thread_report(void);


/* Define the initialization prototype.  */

void            tm_basic_processing_initialize(void);


/* Define main entry point.  */

void main()
{

    /* Initialize the test.  */
    tm_initialize(tm_basic_processing_initialize);
}


/* Define the basic processing test initialization.  */

void  tm_basic_processing_initialize(void)
{

    /* Create thread 0 at priority 10.  */
    tm_thread_create(0, 10, tm_basic_processing_thread_0_entry);

    /* Resume thread 0.  */
    tm_thread_resume(0);

    /* Create the reporting thread. It will preempt the other 
       threads and print out the test results.  */
    tm_thread_create(5, 2, tm_basic_processing_thread_report);
    tm_thread_resume(5);
}


/* Define the basic processing thread.  */
void  tm_basic_processing_thread_0_entry(void)
{

int     i;

    /* Initialize the test array.   */
    for (i = 0; i < 1024; i++)
    {

        /* Clear the basic processing array.  */
        tm_basic_processing_array[i] =  0;
    }

    while(1)
    {

        /* Loop through the basic processing array, add the previous 
           contents with the contents of the tm_basic_processing_counter
           and xor the result with the previous value...   just to eat 
           up some time.  */
        for (i = 0; i < 1024; i++)
        {

            /* Update each array entry.  */
            tm_basic_processing_array[i] =  (tm_basic_processing_array[i] + tm_basic_processing_counter) ^ tm_basic_processing_array[i];
        }

        /* Increment the basic processing counter.  */
        tm_basic_processing_counter++;
    }
}


/* Define the basic processing reporting thread.  */
void  tm_basic_processing_thread_report(void)
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
        printf("**** Thread-Metric Basic Single Thread Processing Test **** Relative Time: %lu\n", relative_time);

        /* See if there are any errors.  */
        if (tm_basic_processing_counter == last_counter)
        {

            printf("ERROR: Invalid counter value(s). Basic processing thread died!\n");
        }

        /* Show the time period total.  */
        printf("Time Period Total:  %lu\n\n", tm_basic_processing_counter - last_counter);
#endif

        /* Save the last counter.  */
        last_counter =  tm_basic_processing_counter;
    }
}
