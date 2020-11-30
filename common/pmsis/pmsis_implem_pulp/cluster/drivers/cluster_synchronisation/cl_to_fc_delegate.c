/*
 * Copyright (c) 2019, GreenWaves Technologies, Inc.
 * Copyright (c) 2020, ETH Zurich
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of GreenWaves Technologies, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "pmsis.h"
#include "pmsis/rtos/event_kernel/event_kernel.h"
#include "pmsis/cluster/cluster_sync/cl_to_fc_delegate.h"
#include "pmsis/implem/hal/hal.h"
#include "pmsis/implem/drivers/pmsis_it.h"

volatile uint32_t g_task;

extern struct cluster_driver_data *__per_cluster_data[];
void cluster_cl_event_handler(void);

HANDLER_WRAPPER_LIGHT(cluster_cl_event_handler);

/**
 * @ingroup groupCluster
 */

/**
 * @defgroup FC_Delegate
 * Delegation of task to FC by cluster API
 */

/**
 * @addtogroup FC_Delegate
 * @{
 */

/**@{*/

void cl_notify_fc_event_handler(void)
{
    volatile struct cluster_driver_data *data = __per_cluster_data[0];
    pmsis_event_push(data->event_kernel, (pi_task_t*)g_task);
    hal_compiler_barrier();
    g_task = 0xdeadbeefUL;
    hal_eu_cluster_evt_trig_set(FC_NOTIFY_CLUSTER_EVENT,0);
}


/** \brief send an opaque task structure for FC
 *
 * FC will execute task according to the opaque structure
 * (callback, driver access...)
 * \param        opaque argument for the fc
 * Side effect: Uses cluster mutex
 */
void pi_cl_send_task_to_fc(pi_task_t *task)
{
    hal_eu_mutex_lock(0);
    while(g_task != 0xdeadbeefUL)
    {
        hal_compiler_barrier();
        hal_eu_evt_mask_wait_and_clr(1<<FC_NOTIFY_CLUSTER_EVENT);
        hal_compiler_barrier();
    }
    g_task = (uint32_t)task;
    hal_eu_fc_evt_trig_set(CLUSTER_NOTIFY_FC_IRQN, 0);
    hal_eu_mutex_unlock(0);
}

void mc_fc_delegate_init(void *arg)
{
    g_task = 0xdeadbeefUL;
    /* Activate interrupt handler for FC when cluster want to push a task to FC */
    NVIC_EnableIRQ(CLUSTER_NOTIFY_FC_IRQN);
    return;
}

void cl_wait_task(uint8_t *done)
{
    while ((*(volatile uint8_t *) done) == 0)
    {
        hal_eu_evt_mask_wait_and_clr(1 << FC_NOTIFY_CLUSTER_EVENT);
    }
}

void cl_notify_task_done(uint8_t *done, uint8_t cluster_id)
{
    (*(volatile uint8_t *) done) = 1;
    hal_eu_cluster_evt_trig_set(FC_NOTIFY_CLUSTER_EVENT, 0);
}
