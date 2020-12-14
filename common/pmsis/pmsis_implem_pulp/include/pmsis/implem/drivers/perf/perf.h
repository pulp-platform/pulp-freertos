/*
 * Copyright 2020 GreenWaves Technologies
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
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __PI_PERF_H__
#define __PI_PERF_H__

#include "pmsis/implem/drivers/perf/perf_internal.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Driver data
 *****************************************************************************/

/*******************************************************************************
 * Function declaration
 ******************************************************************************/

static inline void pi_perf_conf(unsigned events)
{
    #if (FEATURE_CLUSTER == 1)
    if (!pi_is_fc())
    {
        __pi_perf_cl_conf(events);
    }
    else
    #endif  /* FEATURE_CLUSTER */
    {
        __pi_perf_fc_conf(events);
    }
}

static inline void pi_perf_reset()
{
    /* Reset all performance counters to 0. */
    #if (FEATURE_CLUSTER == 1)
    if (!pi_is_fc())
    {
        __pi_perf_cl_reset();
    }
    else
    #endif  /* FEATURE_CLUSTER */
    {
        __pi_perf_fc_reset();
    }
}

static inline void pi_perf_start()
{
    /* Initialize timer if needed and start counters. */
    #if (FEATURE_CLUSTER == 1)
    if (!pi_is_fc())
    {
        __pi_perf_cl_start();
    }
    else
    #endif  /* FEATURE_CLUSTER */
    {
        __pi_perf_fc_start();
    }
}

static inline void pi_perf_stop()
{
    /* Stop counters and timers, and save values. */
    #if (FEATURE_CLUSTER == 1)
    if (!pi_is_fc())
    {
        __pi_perf_cl_stop();
    }
    else
    #endif  /* FEATURE_CLUSTER */
    {
        __pi_perf_fc_stop();
    }
}

static inline unsigned int pi_perf_read(int id)
{
    #if (FEATURE_CLUSTER == 1)
    if (!pi_is_fc())
    {
        return __pi_perf_cl_read(id);
    }
    else
    #endif  /* FEATURE_CLUSTER */
    {
        return __pi_perf_fc_read(id);
    }
}

#endif  /* __PI_PERF_H__ */
