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

#include "pmsis/targets/target.h"
#include "pmsis/implem/drivers/perf/perf_internal.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

#define CSR_PCMR_DISABLE    ( 0x0 ) /* Disable Performance counters. */
#define CSR_PCMR_ENABLE     ( 0x1 ) /* Enable Performance counters. */
#define CSR_PCMR_SATURATE   ( 0x2 ) /* Activate counter saturation. */
#define PCMR_ACTIVE         ( CSR_PCMR_ENABLE )

#define PERF_TIMER_FC       ( FC_TIMER_1 ) /* FC Timer for perf counter. */
#define PERF_TIMER_CL       ( CL_TIMER_0 ) /* CL Timer for perf counter. */

/*******************************************************************************
 * Driver data
 *****************************************************************************/

static perf_t fc_perf_val = {0};

#if defined(FEATURE_CLUSTER)
static perf_t cl_perf_val[ARCHI_CLUSTER_NB_PE] = {0};
#endif  /* FEATURE_CLUSTER */

/*******************************************************************************
 * Internal functions
 ******************************************************************************/
static void __pi_perf_enable_timer(timer_e timer)
{
    timer_cfg_u cfg = {0};
    cfg.field.enable = 1;
    cfg.field.reset = 1;
    cfg.field.irq_en = 0;
    cfg.field.mode = 1;
    cfg.field.one_shot = 0;
    cfg.field.presc_en = 0;
    cfg.field.clk_source = 0;
    /* Init timer. */
    pi_timer_init(timer, cfg, 0xFFFFFFFF);
}

/* For each event in mask, save its counter from PCCR. */
static void __pi_perf_save(perf_t *perf, timer_e timer)
{
    uint32_t perf_mask = perf->perf_mask;
    /* Timer cycles. */
    if (perf_mask & (1 << PI_PERF_CYCLES))
    {
        perf_mask &= ~(1 << PI_PERF_CYCLES);
        perf->perf_counter[PI_PERF_CYCLES] = pi_timer_value_read(timer);
    }

    do
    {
        uint32_t event = __FL1(perf_mask);
        perf_mask &= ~(1 << event);
        perf->perf_counter[event] = __pi_perf_counter_get(event);
    } while (perf_mask);
}

/*******************************************************************************
 * API implementation
 ******************************************************************************/

/* FC */
void __pi_perf_fc_conf(uint32_t events)
{
    fc_perf_val.perf_mask = events;
    /* Set Performance Counter Event Register(PCER) with given events mask. */
    __pi_perf_mask_events_set(fc_perf_val.perf_mask & ~(1 << PI_PERF_CYCLES));
}

void __pi_perf_fc_reset()
{
    /* Reset Timer counter. */
    if (fc_perf_val.perf_mask & 1 << PI_PERF_CYCLES)
    {
        pi_timer_reset(PERF_TIMER_FC);
    }
    /* Reset all Performace Counter Counter Register(PCCR) to 0. */
    __pi_perf_counters_reset();
}

void __pi_perf_fc_start()
{
    /* Start timer if used. */
    if (fc_perf_val.perf_mask & (1 << PI_PERF_CYCLES))
    {
        /* TODO : Activate timers. */
        __pi_perf_enable_timer(PERF_TIMER_FC);
    }
    /* Reset counters. */
    __pi_perf_counters_reset();

    /* Enable performance counters. */
    __pi_perf_counter_enable(CSR_PCMR_ENABLE | CSR_PCMR_SATURATE);
}

void __pi_perf_fc_stop()
{
    /* Disable performance counters. */
    __pi_perf_counter_enable(CSR_PCMR_DISABLE);

    /* Disable timer if used. */
    if (fc_perf_val.perf_mask & (1 << PI_PERF_CYCLES))
    {
        /* TODO : disable timer. */
        pi_timer_stop(PERF_TIMER_FC);
    }

    /* Save counters value. */
    __pi_perf_save(&fc_perf_val, PERF_TIMER_FC);
}

uint32_t __pi_perf_fc_read(int id)
{
    /* Save counters value. */
    __pi_perf_save(&fc_perf_val, PERF_TIMER_FC);
    return fc_perf_val.perf_counter[id];
}

#if defined(FEATURE_CLUSTER)
void __pi_perf_cl_conf(uint32_t events)
{
    uint8_t cid = pi_core_id();
    cl_perf_val[cid].perf_mask = events;
    /* Set Performance Counter Event Register(PCER) with given events mask. */
    __pi_perf_mask_events_set(cl_perf_val[cid].perf_mask & ~(1 << PI_PERF_CYCLES));
}

void __pi_perf_cl_reset()
{
    /* Reset Timer counter. */
    uint8_t cid = pi_core_id();
    if (cid == 0)
    {
        if (cl_perf_val[cid].perf_mask & 1 << PI_PERF_CYCLES)
        {
            pi_timer_reset(PERF_TIMER_CL);
        }
    }
    /* Reset all Performace COunter Counter Register(PCCR) to 0. */
    __pi_perf_counters_reset();
}

void __pi_perf_cl_start()
{
    uint8_t cid = pi_core_id();

    /* Start timer if used. */
    if (cl_perf_val[cid].perf_mask & (1 << PI_PERF_CYCLES))
    {
        /* TODO : Activate timers. */
        __pi_perf_enable_timer(PERF_TIMER_CL);
    }
    /* Reset counters. */
    __pi_perf_counters_reset();

    /* Enable performance counters. */
    __pi_perf_counter_enable(CSR_PCMR_ENABLE | CSR_PCMR_SATURATE);
}

void __pi_perf_cl_stop()
{
    uint8_t cid = pi_core_id();

    /* Disable performance counters. */
    __pi_perf_counter_enable(CSR_PCMR_DISABLE);

    /* Disable timer if used. */
    if (cl_perf_val[cid].perf_mask & (1 << PI_PERF_CYCLES))
    {
        /* TODO : disable timer. */
        pi_timer_stop(PERF_TIMER_CL);
    }

    /* Save counters value. */
    __pi_perf_save(&cl_perf_val[cid], PERF_TIMER_CL);
}

uint32_t __pi_perf_cl_read(int id)
{
    uint8_t cid = pi_core_id();
    /* Save counters value. */
    __pi_perf_save(&cl_perf_val[cid], PERF_TIMER_CL);
    return cl_perf_val[cid].perf_counter[id];
}

#endif  /* FEATURE_CLUSTER */
