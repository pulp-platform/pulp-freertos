/*
 * Copyright (C) 2018 ETH Zurich, University of Bologna and GreenWaves Technologies
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "cluster/cl_to_fc_delegate.h"
#include "cluster/cl_pmsis_types.h"
#include "cluster/cluster_data.h"
#include "cluster/event_unit.h"
#include "pmsis_task.h"
#include "target.h"
#include "irq.h"
#include "events.h"

#if defined MEASURE_ACTIVE && defined PCF_ASYNC
#include "tgt_dbg_measure.h"
#endif

extern struct cluster_driver_data *__per_cluster_data[];

void cl_notify_fc_event_handler(void)
{
	struct cluster_driver_data *data = __per_cluster_data[0];
	struct pi_task *task = data->task_to_fc;
	pi_callback_func_t callback_func = NULL;
	/* Light callback executed now. */
	if ((uint32_t)task & 0x1) {
		task = (pi_task_t *)((uint32_t)task & ~0x1);
		callback_func = (pi_callback_func_t)task->arg[0];
		callback_func((void *)task->arg[1]);
		pi_task_release(task);
	}
	/* Push pi_task callback to event kernel. */
	else {
		/* TODO: callbacks are currently not supported */
		pi_task_push(task);
		/* TODO: this might not be entirly correct. The sink that consumes the
		 * tasks should release it */
		pi_task_release(task);
	}
	hal_compiler_barrier();
	data->task_to_fc = NULL;
	hal_eu_cluster_evt_trig_set(FC_NOTIFY_CLUSTER_EVENT, 0);
}

void pi_cl_send_task_to_fc(pi_task_t *task)
{
	#if defined PCF_ASYNC && defined MEASURE_ACTIVE
    	timerBuffer[Timerindex++] = (Timer_Data_t) {'4',lMeasureReadCsr( MEASURE_ZEROING )};
	#endif

	hal_eu_mutex_lock(0);
	struct cluster_driver_data *data = __per_cluster_data[0];
	while (data->task_to_fc != NULL) {
		hal_compiler_barrier();
		hal_eu_evt_mask_wait_and_clr(1 << FC_NOTIFY_CLUSTER_EVENT);
		hal_compiler_barrier();
	}
	data->task_to_fc = task;
	hal_eu_fc_evt_trig_set(CLUSTER_TO_FC_NOTIFY_IRQN, 0);
	hal_eu_mutex_unlock(0);

	#if defined PCF_ASYNC && defined MEASURE_ACTIVE
    	timerBuffer[Timerindex++] = (Timer_Data_t) {'5',lMeasureReadCsr( MEASURE_ZEROING )};
	#endif
}

void mc_fc_delegate_init(void *arg)
{
	/* Activate interrupt handler for FC when cluster want to push a task to FC */
	irq_enable(CLUSTER_TO_FC_NOTIFY_IRQN);
	irq_clint_enable(CLUSTER_TO_FC_NOTIFY_IRQN);
}

void cl_wait_task(uint8_t *done)
{
	/* while ((*(volatile uint8_t *) done) == 0) */
	/* { */
	/*     hal_eu_evt_mask_wait_and_clr(1 << FC_NOTIFY_CLUSTER_EVENT); */
	/* } */
}

void cl_notify_task_done(uint8_t *done, uint8_t cluster_id)
{
	/* (*(volatile uint8_t *) done) = 1; */
	/* hal_eu_cluster_evt_trig_set(FC_NOTIFY_CLUSTER_EVENT, 0); */
}
