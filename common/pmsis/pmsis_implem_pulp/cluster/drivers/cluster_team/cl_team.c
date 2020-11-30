/*
 * Copyright (C) 2019 ETH Zurich, University of Bologna and GreenWaves Technologies
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
#include "pmsis/cluster/cluster_team/cl_team.h"
#include "pmsis/cluster/cluster_sync/fc_to_cl_delegate.h"
#include "pmsis/implem/hal/hal.h"
/**
 * @ingroup groupCluster
 */

/**
 * @defgroup Team Team synchronization primitives
 *
 * Once a cluster entry point has been entered, after it has been enqueued by the fabric controller, all the following primitives can be used
 * to do multi-core processing, in a fork-join manner (like OMP parallel primitive).
 *
 * The execution can first be forked in order to activate more cores, and then synchronized together using barriers.
 */

/**
 * @addtogroup Team
 * @{
 */

/**@{*/

static uint32_t team_nb_cores = 0;

/** \brief Fork the execution of the calling core.
 *
 * Calling this function will create a team of workers and call the specified entry point
 * to start multi-core processing.
 * The team parameters (number of cores and stacks) are by default the ones
 * configured when calling
 * the cluster entry point from the fabric controller. It is possible to use
 * different parameters
 * when doing a new fork. If this is done the new parameters will become the
 * new default ones.
 * If the number of cores is not provided (i.e. is zero), the number of cores
 * of the previous
 * fork will be reused. Doing this has less runtime overhead.
 *
 * \param        cores_mask The mask of cores which will enter the entry point.
 * If it is zero, this will reuse the cores_mask of the previous fork or the default.
 * \param        entry The function entry point to be executed by all cores of the team.
 * \param        arg    The argument of the function entry point.
 */
void pi_cl_team_fork(int nb_cores, void (*entry)(void *), void *arg)
{
    uint32_t core_mask = 0;
    if (!nb_cores || ((uint32_t) nb_cores == team_nb_cores))
    {
        if (!team_nb_cores)
        {
            team_nb_cores = (uint32_t) ARCHI_CLUSTER_NB_PE;
            core_mask = ((1 << team_nb_cores)-1);
            hal_eu_dispatch_team_config(core_mask & GAP_CLUSTER_WITHOUT_CORE0_MASK);
        }
        else
        {
            core_mask = ((1 << team_nb_cores)-1);
        }
    }
    else
    {
        team_nb_cores = (uint32_t) nb_cores;
        core_mask = ((1 << team_nb_cores)-1);
        hal_eu_dispatch_team_config(core_mask & GAP_CLUSTER_WITHOUT_CORE0_MASK);
    }
    hal_eu_barrier_setup(core_mask);
    hal_eu_dispatch_push((uint32_t) entry);
    hal_eu_dispatch_push((uint32_t) arg);
    entry(arg);
    hal_eu_barrier_trigger_wait_clear();
}


/** \brief Execute a barrier between all cores of the team.
 *
 * This will block the execution of each calling core until all cores have reached the barrier.
 * The set of cores participating in the barrier is the one created with the last fork.
 * Each core of the team must execute the barrier exactly once for all cores to be able to go through the barrier.
 */
void pi_cl_team_barrier(int barrier_id)
{
    hal_evt_read32((uint32_t)&EU_BARRIER_DEMUX(0)->TRIGGER_WAIT_CLEAR, 0);
}



/** \brief Enter a critical section.
 *
 * This will block the execution of the calling core until it can execute the following section of code alone.
 * This will also prevent all other cores of the team to execute the following code until
 * rt_team_critical_exit is called.
 */
void pi_cl_team_critical_enter()
{
    hal_eu_mutex_lock(0);
}



/** \brief Exit a critical section.
 *
 * This will exit the critical code and let other cores executing it..
 */
void pi_cl_team_critical_exit()
{
    hal_eu_mutex_unlock(0);
}

int pi_cl_team_nb_cores()
{
    return team_nb_cores;
}

//!@}

/**
 * @} end of Team group
 */
