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

#ifndef __PI_HAL_SOC_CTRL_PAD_H__
#define __PI_HAL_SOC_CTRL_PAD_H__

#include "pmsis/targets/target.h"

#define ARCHI_PAD_NB_PADFUNC_REG      (0x4)
#define ARCHI_PAD_NB_PAD_PER_PADFUNC  (ARCHI_NB_PAD >> ARCHI_PAD_NB_PADFUNC_REG)
#define ARCHI_PAD_NB_PADCFG_REG       (0x10)

#define APB_SOC_STATUS_EOC_SHIFT      (31)

/* Corestatus register. */
static inline void soc_ctrl_corestatus_set(int32_t value)
{
    hal_write32(&(soc_ctrl->corestatus), (1 << APB_SOC_STATUS_EOC_SHIFT) | value);
    //hal_write32(&(soc_ctrl->corestatus), value);
}

static inline uint32_t soc_ctrl_corestatus_get(void)
{
    return hal_read32(&(soc_ctrl->corestatus));
}


/* Safe_padfun register. */
static inline void soc_ctrl_safe_padfun_set(uint8_t reg_num, uint32_t value)
{
    hal_write32(&(soc_ctrl->safe_padfun[reg_num]), value);
}

static inline uint32_t soc_ctrl_safe_padfun_get(uint8_t reg_num)
{
    return hal_read32(&(soc_ctrl->safe_padfun[reg_num]));
}


/* Safe_sleeppadcfg register. */
static inline void soc_ctrl_safe_sleeppadcfg_set(uint8_t reg_num, uint32_t value)
{
    hal_write32(&(soc_ctrl->safe_sleeppadcfg[reg_num]), value);
}

static inline uint32_t soc_ctrl_safe_sleeppadcfg_get(uint8_t reg_num)
{
    return hal_read32(&(soc_ctrl->safe_sleeppadcfg[reg_num]));
}


/* Safe_padsleep register. */
static inline void soc_ctrl_safe_padsleep_set(uint32_t value)
{
    hal_write32(&(soc_ctrl->safe_padsleep), value);
}

static inline uint32_t soc_ctrl_safe_padsleep_get()
{
    return hal_read32(&(soc_ctrl->safe_padsleep));
}


/* Safe_padcfg register. */
static inline void soc_ctrl_safe_padcfg_set(uint8_t reg_num, uint32_t value)
{
    hal_write32(&(soc_ctrl->safe_padcfg[reg_num]), value);
}

static inline uint32_t soc_ctrl_safe_padcfg_get(uint8_t reg_num)
{
    return hal_read32(&(soc_ctrl->safe_padcfg[reg_num]));
}


/*! Safe_padfun. */
/* Pad function configuration. */
static inline void hal_pad_set_function(uint8_t pad, uint8_t func)
{
    /* Get reg number and its position in the register. */
    uint32_t pad_reg = (pad >> 4), pad_pos = (pad & 0x0F);
    uint32_t mask = 0x3, shift = (pad_pos << 1);
    uint32_t val = soc_ctrl_safe_padfun_get(pad_reg);
    val = ((val & ~(mask << shift)) | (func << shift));
    soc_ctrl_safe_padfun_set(pad_reg, val);
}

static inline void hal_pad_set_padfunc(uint8_t pad_reg, uint32_t value)
{
    soc_ctrl_safe_padfun_set(pad_reg, value);
}

static inline uint32_t hal_pad_get_padfunc(uint8_t pad_reg)
{
    return soc_ctrl_safe_padfun_get(pad_reg);
}


/*! Safe_sleeppadcfg. */
/* Sleeppad direction configuration. */
static inline void hal_sleeppad_set_direction(uint8_t pad, uint8_t dir)
{
    /* Get reg number and its position in the register. */
    uint32_t pad_reg = (pad >> 4), pad_pos = (pad & 0x0F);
    uint32_t mask = 0x1, shift = (pad_pos << 1);
    uint32_t val = soc_ctrl_safe_sleeppadcfg_get(pad_reg);
    val = ((val & ~(mask << shift)) | (dir << shift));
    soc_ctrl_safe_sleeppadcfg_set(pad_reg, val);
}

/* Sleeppad state configuration. */
static inline void hal_sleeppad_set_state(uint8_t pad, uint8_t state)
{
    /* Get reg number and its position in the register. */
    uint32_t pad_reg = (pad >> 4), pad_pos = (pad & 0x0F);
    uint32_t mask = 0x2, shift = (pad_pos << 1);
    uint32_t val = soc_ctrl_safe_sleeppadcfg_get(pad_reg);
    val = ((val & ~(mask << shift)) | ((state << 1) << shift));
    soc_ctrl_safe_sleeppadcfg_set(pad_reg, val);
}

/* Sleeppad state & dir configuration. */
static inline void hal_sleeppad_set_configuration(uint8_t pad, uint8_t dir, uint8_t state)
{
    /* Get reg number and its position in the register. */
    uint32_t pad_reg = (pad >> 4), pad_pos = (pad & 0x0F);
    uint32_t mask = 0x3, shift = (pad_pos << 1);
    uint32_t val = soc_ctrl_safe_sleeppadcfg_get(pad_reg);
    val = ((val & ~(mask << shift)) | (((state << 1) | dir) << shift));
    soc_ctrl_safe_sleeppadcfg_set(pad_reg, val);
}


/*! Safe_padsleep. */
/* Enable sleep mode for pads. */
static inline void hal_padsleep_enable(uint8_t enable)
{
    uint32_t mask = (uint32_t) SOC_CTRL_SAFE_PADSLEEP_EN_MASK, shift = 0;
    uint32_t val = soc_ctrl_safe_padsleep_get();
    val = ((val & ~(mask << shift)) | (enable << shift));
    soc_ctrl_safe_padsleep_set(val);
}


/*! Safe_padcfg. */
/* Set pull activation of a pad. */
static inline void hal_padcfg_pull_enable(uint8_t pad, uint8_t pe)
{
    /* Get reg number and its position in the register. */
    uint32_t pad_reg = (pad >> 2), pad_pos = (pad & 0x03);
    uint32_t mask = 0x1, shift = (pad_pos << 3);
    uint32_t val = soc_ctrl_safe_padcfg_get(pad_reg);
    val = ((val & ~(mask << shift)) | (pe << shift));
    soc_ctrl_safe_padcfg_set(pad_reg, val);
}

/* Set drive strength of a pad. */
static inline void hal_padcfg_drive_strength_set(uint8_t pad, uint8_t ds)
{
    /* Get reg number and its position in the register. */
    uint32_t pad_reg = (pad >> 2), pad_pos = (pad & 0x03);
    uint32_t mask = 0x2, shift = (pad_pos << 3);
    uint32_t val = soc_ctrl_safe_padcfg_get(pad_reg);
    val = ((val & ~(mask << shift)) | ((ds << 1) << shift));
    soc_ctrl_safe_padcfg_set(pad_reg, val);
}

/* Padcfg pull activation & drive strength configuration. */
static inline void hal_padcfg_set_configuration(uint8_t pad, uint8_t pe, uint8_t ds)
{
    /* Get reg number and its position in the register. */
    uint32_t pad_reg = (pad >> 2), pad_pos = (pad & 0x03);
    uint32_t mask = 0x3, shift = (pad_pos << 3);
    uint32_t val = soc_ctrl_safe_padcfg_get(pad_reg);
    val = ((val & ~(mask << shift)) | (((ds << 1) | pe) << shift));
    soc_ctrl_safe_padcfg_set(pad_reg, val);
}

#endif  /* __PI_HAL_SOC_CTRL_PAD_H__ */
