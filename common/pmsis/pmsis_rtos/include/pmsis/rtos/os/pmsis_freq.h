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

#ifndef __PI_FREQ_H__
#define __PI_FREQ_H__

#include "pmsis.h"
#include "pmsis/implem/drivers/drivers.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Driver data
 ******************************************************************************/

/*******************************************************************************
 * Function declaration
 ******************************************************************************/

/*******************************************************************************
 * API implementation
 ******************************************************************************/

static inline uint32_t pi_freq_get(pi_freq_domain_e domain)
{
    switch(domain)
    {
    case PI_FREQ_DOMAIN_FC :
        return pi_fll_get_frequency(FLL_SOC, 0);
    case PI_FREQ_DOMAIN_CL :
        return pi_fll_get_frequency(FLL_CLUSTER, 0);
    case PI_FREQ_DOMAIN_PERIPH :
        return pi_fll_get_frequency(FLL_SOC, 0);
    default :
        return 0;
    }
}

static inline int32_t pi_freq_set(pi_freq_domain_e domain, uint32_t freq)
{
    int32_t retval = -1;
    switch(domain)
    {
    case PI_FREQ_DOMAIN_FC :
        retval = pi_fll_set_frequency(FLL_SOC, freq, 1);
        break;
    case PI_FREQ_DOMAIN_CL :
        retval = pi_fll_set_frequency(FLL_CLUSTER, freq, 1);
        break;
    case PI_FREQ_DOMAIN_PERIPH :
        retval = pi_fll_set_frequency(FLL_SOC, freq, 1);
        break;
    default :
        retval = -1;
    }

    return ((retval == -1) ? -1 : 0);
}

#endif  /* __PI_FREQ_H__ */
