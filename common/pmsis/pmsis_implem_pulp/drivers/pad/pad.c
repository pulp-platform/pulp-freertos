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

#include "pmsis/drivers/pad.h"
#include "pmsis.h"
#include "pmsis/implem/hal/hal.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Driver data
 *****************************************************************************/

/*******************************************************************************
 * API implementation
 ******************************************************************************/

void pi_pad_print()
{
    DEBUG_PRINTF("\nPadfun :\n");
    for (uint32_t i=0; i<(uint32_t) ARCHI_PAD_NB_PADFUNC_REG; i+=4)
    {
        DEBUG_PRINTF("%x %x %x %x\n",
                     soc_ctrl_safe_padfun_get(i + 0), soc_ctrl_safe_padfun_get(i + 1),
                     soc_ctrl_safe_padfun_get(i + 2), soc_ctrl_safe_padfun_get(i + 3));
    }
    DEBUG_PRINTF("Padcfg :\n");
    for (uint32_t i=0; i<(uint32_t) ARCHI_PAD_NB_PADCFG_REG; i+=4)
    {
        DEBUG_PRINTF("%x %x %x %x\n",
                     soc_ctrl_safe_padcfg_get(i + 0), soc_ctrl_safe_padcfg_get(i + 1),
                     soc_ctrl_safe_padcfg_get(i + 2), soc_ctrl_safe_padcfg_get(i + 3));
    }
}

void pi_pad_set_function(pi_pad_e pad, pi_pad_func_e function)
{
    hal_pad_set_function(pad, function);
    #ifdef DEBUG
    DEBUG_PRINTF("Pad: %d func: %d\n", pad, function);
    pi_pad_print();
    #endif
}

void pi_pad_init(uint32_t pad_values[])
{
    for (uint32_t pad_reg = 0; pad_reg < (uint32_t) ARCHI_PAD_NB_PADFUNC_REG; pad_reg++)
    {
        hal_pad_set_padfunc(pad_reg, pad_values[pad_reg]);
    }
    #ifdef DEBUG
    pi_pad_print();
    #endif
}

void pi_pad_set_configuration(pi_pad_e pad, pi_pad_flags_e cfg)
{
    hal_padcfg_set_configuration(pad, (cfg >> PI_PAD_PULL_OFFSET), (cfg >> PI_PAD_DRIVE_OFFSET));
}
