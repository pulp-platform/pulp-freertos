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
/**   Message Processing Test                                             */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/

#include "tm_api.h"
//#include "stdio.h"


/* Define the counters used in the demo application...  */

volatile unsigned long   tm_message_processing_counter;
unsigned long   tm_message_sent[4];
unsigned long   tm_message_received[4];


/* Define the test thread prototypes.  */

void            tm_message_processing_thread_0_entry(void);


/* Define the reporting thread prototype.  */

void            tm_message_processing_thread_report(void);


/* Define the initialization prototype.  */

void            tm_message_processing_initialize(void);


/* Define main entry point.  */

int main()
{

    /* Initialize the test.  */
    tm_initialize(tm_message_processing_initialize);
    return 0;
}


/* Define the message processing test initialization.  */

void  tm_message_processing_initialize(void)
{

    /* Create thread 0 at priority 10.  */
    tm_thread_create(0, 10, tm_message_processing_thread_0_entry);

    /* Resume thread 0.  */
    tm_thread_resume(0);

    /* Create a queue for the message passing.  */
    tm_queue_create(0);

    /* Create the reporting thread. It will preempt the other 
       threads and print out the test results.  */
    tm_thread_create(5, 2, tm_message_processing_thread_report);
    tm_thread_resume(5);
}


/* Define the message processing thread.  */
void  tm_message_processing_thread_0_entry(void)
{

    /* Initialize the source message.   */
    tm_message_sent[0] =  0x11112222;
    tm_message_sent[1] =  0x33334444;
    tm_message_sent[2] =  0x55556666;
    tm_message_sent[3] =  0x77778888;

    while(1)
    {

        /* Send a message to the queue.  */
        tm_queue_send(0, tm_message_sent);

        /* Receive a message from the queue.  */
        tm_queue_receive(0, tm_message_received);

        /* Check for invalid message.  */
        if (tm_message_received[3] != tm_message_sent[3])
            break;

        /* Increment the last word of the 16-byte message.  */
        tm_message_sent[3]++;

        /* Increment the number of messages sent and received.  */
        tm_message_processing_counter++;
    }
}


/* Define the message test reporting thread.  */
void  tm_message_processing_thread_report(void)
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
        printf("**** Thread-Metric Message Processing Test **** Relative Time: %lu\n", relative_time);
#endif
        /* See if there are any errors.  */
        if (tm_message_processing_counter == last_counter)
        {
#ifdef ENABLE_PRINTF
            printf("ERROR: Invalid counter value(s). Error sending/receiving messages!\n");
#else
            while(1)
            {

            }
#endif
        }
#ifdef ENABLE_PRINTF
        /* Show the time period total.  */
        printf("Time Period Total:  %lu\n\n", tm_message_processing_counter - last_counter);
#endif
        /* Save the last counter.  */
        last_counter =  tm_message_processing_counter;
    }
}
