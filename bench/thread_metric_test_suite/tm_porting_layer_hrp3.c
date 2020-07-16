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

#include    <kernel.h>
#include    <t_syslog.h>
#include    "kernel_cfg.h"
#include    "target_test.h"

#include    "tm_api.h"
#include    "tm_porting_layer_hrp3.h"


Inline void
svc_perror(const char *file, int_t line, const char *expr, ER ercd)
{
	if (ercd < 0) {
		t_perror(LOG_ERROR, file, line, expr, ercd);
	}
}

#define	SVC_PERROR(expr)	svc_perror(__FILE__, __LINE__, #expr, (expr))

typedef struct {
	ID   taskid;
	void *entry_function;
} thread_t;

static thread_t thread_tab[] = {
	{TM_TASK_0}, {TM_TASK_1}, {TM_TASK_2}, {TM_TASK_3}, {TM_TASK_4}, {TM_TASK_5}
};

void
tm_thread_task(intptr_t exinf)
{
	((void (*)(void)) thread_tab[exinf].entry_function)();
}


/* This function called from main performs basic RTOS initialization, 
   calls the test initialization function, and then starts the RTOS function.  */
void  tm_initialize(void (*test_initialization_function)(void))
{
	(test_initialization_function)();
}


/* This function takes a thread ID and priority and attempts to create the
   file in the underlying RTOS.  Valid priorities range from 1 through 31, 
   where 1 is the highest priority and 31 is the lowest. If successful, 
   the function should return TM_SUCCESS. Otherwise, TM_ERROR should be returned.   */
int  tm_thread_create(int thread_id, int priority, void (*entry_function)(void))
{
	thread_tab[thread_id].entry_function = entry_function;
	SVC_PERROR(act_tsk(thread_tab[thread_id].taskid));
	SVC_PERROR(chg_pri(thread_tab[thread_id].taskid, TM_MAIN_PRIORITY + priority));
	tm_thread_suspend(thread_id);
}


/* This function resumes the specified thread.  If successful, the function should
   return TM_SUCCESS. Otherwise, TM_ERROR should be returned.  */
int  tm_thread_resume(int thread_id)
{
	SVC_PERROR(rsm_tsk(thread_tab[thread_id].taskid));
}


/* This function suspends the specified thread.  If successful, the function should
   return TM_SUCCESS. Otherwise, TM_ERROR should be returned.  */
int  tm_thread_suspend(int thread_id)
{
	SVC_PERROR(sus_tsk(thread_tab[thread_id].taskid));
}


/* This function relinquishes to other ready threads at the same
   priority.  */
void tm_thread_relinquish(void)
{
	SVC_PERROR(rot_rdq(TPRI_SELF));
}


/* This function suspends the specified thread for the specified number
   of seconds.  If successful, the function should return TM_SUCCESS. 
   Otherwise, TM_ERROR should be returned.  */
void tm_thread_sleep(int seconds)
{
	SVC_PERROR(dly_tsk(seconds * 1000U * 1000U));
}


/* This function creates the specified queue.  If successful, the function should
   return TM_SUCCESS. Otherwise, TM_ERROR should be returned.  */
int  tm_queue_create(int queue_id)
{

}


/* This function sends a 16-byte message to the specified queue.  If successful, 
   the function should return TM_SUCCESS. Otherwise, TM_ERROR should be returned.  */
int  tm_queue_send(int queue_id, unsigned long *message_ptr)
{
	SVC_PERROR(snd_mbf(TM_MBF_ID, message_ptr, 16U));
}


/* This function receives a 16-byte message from the specified queue.  If successful, 
   the function should return TM_SUCCESS. Otherwise, TM_ERROR should be returned.  */
int  tm_queue_receive(int queue_id, unsigned long *message_ptr)
{
	SVC_PERROR(rcv_mbf(TM_MBF_ID, message_ptr));
}


/* This function creates the specified semaphore.  If successful, the function should
   return TM_SUCCESS. Otherwise, TM_ERROR should be returned.  */
int  tm_semaphore_create(int semaphore_id)
{

}


/* This function gets the specified semaphore.  If successful, the function should
   return TM_SUCCESS. Otherwise, TM_ERROR should be returned.  */
int  tm_semaphore_get(int semaphore_id)
{
	ER ercd;
	SVC_PERROR(ercd = pol_sem(TM_SEM_ID));
	if (ercd == E_OK) {
		return TM_SUCCESS;
	}
	return TM_ERROR;
}


/* This function waits the specified semaphore.  If successful, the function should
   return TM_SUCCESS. Otherwise, TM_ERROR should be returned.  */
int  tm_semaphore_wait(int semaphore_id)
{
	ER ercd;
	SVC_PERROR(ercd = wai_sem(TM_SEM_ID));
	if (ercd == E_OK) {
		return TM_SUCCESS;
	}
	return TM_ERROR;
}


/* This function puts the specified semaphore.  If successful, the function should
   return TM_SUCCESS. Otherwise, TM_ERROR should be returned.  */
int  tm_semaphore_put(int semaphore_id)
{
	ER ercd;
	SVC_PERROR(ercd = sig_sem(TM_SEM_ID));
	if (ercd == E_OK) {
		return TM_SUCCESS;
	}
	return TM_ERROR;
}


/* This function puts the specified semaphore.  If successful, the function should
   return TM_SUCCESS. Otherwise, TM_ERROR should be returned.  */
int  tm_semaphore_put_from_isr(int semaphore_id)
{
	ER ercd;
	SVC_PERROR(ercd = SVC_CALL(sig_sem)(TM_SEM_ID));
	if (ercd == E_OK) {
		return TM_SUCCESS;
	}
	return TM_ERROR;
}


/* This function creates the specified memory pool that can support one or more
   allocations of 128 bytes.  If successful, the function should
   return TM_SUCCESS. Otherwise, TM_ERROR should be returned.  */
int  tm_memory_pool_create(int pool_id)
{

}


/* This function allocates a 128 byte block from the specified memory pool.  
   If successful, the function should return TM_SUCCESS. Otherwise, TM_ERROR 
   should be returned.  */
int  tm_memory_pool_allocate(int pool_id, unsigned char **memory_ptr)
{
	SVC_PERROR(get_mpf(TM_MPF_ID, memory_ptr));
}


/* This function releases a previously allocated 128 byte block from the specified 
   memory pool. If successful, the function should return TM_SUCCESS. Otherwise, TM_ERROR 
   should be returned.  */
int  tm_memory_pool_deallocate(int pool_id, unsigned char *memory_ptr)
{
	SVC_PERROR(rel_mpf(TM_MPF_ID, memory_ptr));
}


void tm_interrupt_raise(void)
{
	__int_exception(TM_INTNO);
}
