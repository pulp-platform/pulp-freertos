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
/** Application Interface (API)                                           */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/


/**************************************************************************/ 
/*                                                                        */ 
/*  APPLICATION INTERFACE DEFINITION                       RELEASE        */ 
/*                                                                        */ 
/*    tm_api.h                                            PORTABLE C      */ 
/*                                                           4.2          */ 
/*  AUTHOR                                                                */ 
/*                                                                        */ 
/*    William E. Lamie, Express Logic, Inc.                               */ 
/*                                                                        */ 
/*  DESCRIPTION                                                           */ 
/*                                                                        */ 
/*    This file defines the basic Application Interface (API)             */ 
/*    implementation source code for the Thread-Metrics performance       */
/*    test suite. All service prototypes and data structure definitions   */
/*    are defined in this file.                                           */ 
/*                                                                        */ 
/*  RELEASE HISTORY                                                       */ 
/*                                                                        */ 
/*    DATE              NAME                      DESCRIPTION             */ 
/*                                                                        */ 
/*  03-01-2004     William E. Lamie         Initial Version 4.0           */
/*  03-05-2007     William E. Lamie         CMP Release New Banner 4.1    */ 
/*  05-15-2016     William E. Lamie         Updated comments 4.2          */ 
/*                                                                        */ 
/**************************************************************************/ 

#ifndef  TM_API_H
#define  TM_API_H

/* Determine if a C++ compiler is being used.  If so, ensure that standard
   C is used to process the API information.  */

#ifdef   __cplusplus

/* Yes, C++ compiler is present.  Use standard C.  */
extern   "C" {

#endif

/* Define API constants.  */

#define TM_SUCCESS  0
#define TM_ERROR    1


/* Define the time interval in seconds. This can be changed with a -D compiler option.  */

#ifndef TM_TEST_DURATION
#define TM_TEST_DURATION    30
#endif


/* Define RTOS Neutral APIs. RTOS vendors should fill in the guts of the following
   API. Once this is done the Thread-Metric tests can be successfully run.  */

void   tm_initialize(void (*test_initialization_function)(void));
int    tm_thread_create(int thread_id, int priority, void (*entry_function)(void));
int    tm_thread_resume(int thread_id);
int    tm_thread_suspend(int thread_id);
void   tm_thread_relinquish(void);
void   tm_thread_sleep(int seconds);
int    tm_queue_create(int queue_id);
int    tm_queue_send(int queue_id, unsigned long *message_ptr);
int    tm_queue_receive(int queue_id, unsigned long *message_ptr);
int    tm_semaphore_create(int semaphore_id);
int    tm_semaphore_get(int semaphore_id);
int    tm_semaphore_wait(int semaphore_id);
int    tm_semaphore_put(int semaphore_id);
int    tm_semaphore_put_from_isr(int semaphore_id);
int    tm_memory_pool_create(int pool_id);
int    tm_memory_pool_allocate(int pool_id, unsigned char **memory_ptr);
int    tm_memory_pool_deallocate(int pool_id, unsigned char *memory_ptr);

/* APIs for interrupt handling -- ertl-liyixiao */
void   tm_interrupt_raise();
void   tm_interrupt_processing_handler();
void   tm_interrupt_preemption_handler();

/* Determine if a C++ compiler is being used.  If so, complete the standard
   C conditional started above.  */
#ifdef   __cplusplus
        }
#endif

#endif
