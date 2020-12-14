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

#ifndef __PI_PERF_INTERNAL_H__
#define __PI_PERF_INTERNAL_H__

#include "pmsis/drivers/perf.h"
#include "pmsis/chips/gap8/perf.h"
#include "pmsis/implem/drivers/timer/timer.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Driver data
 *****************************************************************************/

typedef struct
{
    uint32_t perf_mask;                     /*!< Mask of Performance counters being used. */
    uint32_t perf_counter[PCER_EVENTS_NUM]; /*!< Performance counters value. */
} perf_t;

/*******************************************************************************
 * Function declaration
 ******************************************************************************/

/**
 * \brief Configure FC perf counter.
 *
 * This function configures Performance Counter with given events mask.
 *
 * \param events         Events mask.
 */
void __pi_perf_fc_conf(uint32_t events);

/**
 * \brief Reset FC perf counters.
 *
 * This function resets all Performance Counter.
 */
void __pi_perf_fc_reset();

/**
 * \brief Start FC perf counters.
 *
 * This function starts Performance Counter.
 */
void __pi_perf_fc_start();

/**
 * \brief Stop FC perf counters.
 *
 * This function stops Performance Counter.
 */
void __pi_perf_fc_stop();

/**
 * \brief Read FC perf counters value.
 *
 * This function returns Performance Counter for a given event.
 *
 * \param id             ID of the event.
 *
 * \return Value         Counter's value for the given event.
 */
uint32_t __pi_perf_fc_read(int id);

#if defined(FEATURE_CLUSTER)
/**
 * \brief Configure Cluster perf counter.
 *
 * This function configures Performance Counter with given events mask.
 *
 * \param events         Events mask.
 */
void __pi_perf_cl_conf(uint32_t events);

/**
 * \brief Reset Cluster perf counters.
 *
 * This function resets all Performance Counter.
 */
void __pi_perf_cl_reset();

/**
 * \brief Start Cluster perf counters.
 *
 * This function starts Performance Counter.
 */
void __pi_perf_cl_start();

/**
 * \brief Stop Cluster perf counters.
 *
 * This function stops Performance Counter.
 */
void __pi_perf_cl_stop();

/**
 * \brief Read Cluster perf counters value.
 *
 * This function returns Performance Counter for a given event.
 *
 * \param id             ID of the event.
 *
 * \return Value         Counter's value for the given event.
 */
uint32_t __pi_perf_cl_read(int id);

#endif  /* FEATURE_CLUSTER */

/*******************************************************************************
 * Internal functions
 ******************************************************************************/

static inline void __pi_perf_mask_events_set(uint32_t mask)
{
    __PCER_Set(mask);
}

static inline void __pi_perf_counters_reset()
{
    __PCCR31_Set(0);
}

static inline void __pi_perf_counter_enable(uint32_t mask)
{
    __PCMR_Set(mask);
}

static inline uint32_t __pi_perf_counter_get(uint32_t event)
{
    return __PCCRs_Get(event);
}

#endif  /* __PI_PERF_INTERNAL_H__ */
