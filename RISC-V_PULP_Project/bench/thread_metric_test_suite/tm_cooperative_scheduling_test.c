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
/**   Cooperative Scheduling Test                                         */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/

#include "tm_api.h"
//#include "stdio.h"

/* Define the counters used in the demo application...  */

volatile unsigned long   tm_cooperative_thread_0_counter;
volatile unsigned long   tm_cooperative_thread_1_counter;
volatile unsigned long   tm_cooperative_thread_2_counter;
volatile unsigned long   tm_cooperative_thread_3_counter;
volatile unsigned long   tm_cooperative_thread_4_counter;


/* Define the test thread prototypes.  */

void            tm_cooperative_thread_0_entry(void);
void            tm_cooperative_thread_1_entry(void);
void            tm_cooperative_thread_2_entry(void);
void            tm_cooperative_thread_3_entry(void);
void            tm_cooperative_thread_4_entry(void);


/* Define the reporting thread prototype.  */

void            tm_cooperative_thread_report(void);


/* Define the initialization prototype.  */

void            tm_cooperative_scheduling_initialize(void);


/* Define main entry point.  */

int main()
{
  tm_cooperative_thread_0_counter = 
  tm_cooperative_thread_1_counter =
  tm_cooperative_thread_2_counter =
  tm_cooperative_thread_3_counter =
  tm_cooperative_thread_4_counter = 0;
  
    /* Initialize the test.  */
    tm_initialize(tm_cooperative_scheduling_initialize);
    return 0;
}


/* Define the cooperative scheduling test initialization.  */

void  tm_cooperative_scheduling_initialize(void)
{

    /* Create all 5 threads at priority 3.  */
    tm_thread_create(0, 3, tm_cooperative_thread_0_entry);
    tm_thread_create(1, 3, tm_cooperative_thread_1_entry);
    tm_thread_create(2, 3, tm_cooperative_thread_2_entry);
    tm_thread_create(3, 3, tm_cooperative_thread_3_entry);
    tm_thread_create(4, 3, tm_cooperative_thread_4_entry);

    /* Resume all 5 threads.  */
    tm_thread_resume(0);
    tm_thread_resume(1);
    tm_thread_resume(2);
    tm_thread_resume(3);
    tm_thread_resume(4);

    /* Create the reporting thread. It will preempt the other 
       threads and print out the test results.  */
    tm_thread_create(5, 2, tm_cooperative_thread_report);
    tm_thread_resume(5);
}


/* Define the first cooperative thread.  */
void  tm_cooperative_thread_0_entry(void)
{

    while(1)
    {

        /* Relinquish to all other threads at same priority.  */
        tm_thread_relinquish();

        /* Increment this thread's counter.  */
        tm_cooperative_thread_0_counter++;
    }
}

/* Define the second cooperative thread.  */
void  tm_cooperative_thread_1_entry(void)
{

    while(1)
    {

        /* Relinquish to all other threads at same priority.  */
        tm_thread_relinquish();

        /* Increment this thread's counter.  */
        tm_cooperative_thread_1_counter++;
    }
}

/* Define the third cooperative thread.  */
void  tm_cooperative_thread_2_entry(void)
{

    while(1)
    {

        /* Relinquish to all other threads at same priority.  */
        tm_thread_relinquish();

        /* Increment this thread's counter.  */
        tm_cooperative_thread_2_counter++;
    }
}


/* Define the fourth cooperative thread.  */
void  tm_cooperative_thread_3_entry(void)
{

    while(1)
    {

        /* Relinquish to all other threads at same priority.  */
        tm_thread_relinquish();

        /* Increment this thread's counter.  */
        tm_cooperative_thread_3_counter++;
    }
}


/* Define the fifth cooperative thread.  */
void  tm_cooperative_thread_4_entry(void)
{

    while(1)
    {

        /* Relinquish to all other threads at same priority.  */
        tm_thread_relinquish();

        /* Increment this thread's counter.  */
        tm_cooperative_thread_4_counter++;
    }
}


/* Define the cooperative test reporting thread.  */
void  tm_cooperative_thread_report(void)
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
        printf("**** Thread-Metric Cooperative Scheduling Test **** Relative Time: %lu\n", relative_time);
#endif
        /* Calculate the total of all the counters.  */
        total =  tm_cooperative_thread_0_counter + tm_cooperative_thread_1_counter + tm_cooperative_thread_2_counter
                    + tm_cooperative_thread_3_counter + tm_cooperative_thread_4_counter;

        /* Calculate the average of all the counters.  */
        average =  total/5;

        /* See if there are any errors.  */
        if ((tm_cooperative_thread_0_counter < (average - 1)) || 
            (tm_cooperative_thread_0_counter > (average + 1)) ||
            (tm_cooperative_thread_1_counter < (average - 1)) || 
            (tm_cooperative_thread_1_counter > (average + 1)) ||
            (tm_cooperative_thread_2_counter < (average - 1)) || 
            (tm_cooperative_thread_2_counter > (average + 1)) ||
            (tm_cooperative_thread_3_counter < (average - 1)) || 
            (tm_cooperative_thread_3_counter > (average + 1)) ||
            (tm_cooperative_thread_4_counter < (average - 1)) || 
            (tm_cooperative_thread_4_counter > (average + 1)))
        {
#ifdef ENABLE_PRINTF
            printf("ERROR: Invalid counter value(s). Cooperative counters should not be more that 1 different than the average!\n");
#else
            while(1)
            {

            }

#endif
        }

#ifdef ENABLE_PRINTF
        /* Show the time period total.  */
        printf("Time Period Total:  %lu\n\n", total - last_total);
#endif
        /* Save the last total.  */
        last_total =  total;
    }
}
