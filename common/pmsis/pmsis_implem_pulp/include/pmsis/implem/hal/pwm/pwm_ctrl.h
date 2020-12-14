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

#ifndef __PI_HAL_PWM_CTRL_H__
#define __PI_HAL_PWM_CTRL_H__

#include "pmsis/targets/target.h"

/* Event_cfg register. */
static inline void pwm_ctrl_event_cfg_set(uint32_t evt_cfg)
{
    hal_write32(&(pwm_ctrl->event_cfg), evt_cfg);
}

static inline uint32_t pwm_ctrl_event_cfg_get()
{
    return hal_read32(&(pwm_ctrl->event_cfg));
}


/* CG register. */
static inline void pwm_ctrl_cg_set(uint32_t cg)
{
    hal_write32(&(pwm_ctrl->cg), cg);
}

static inline uint32_t pwm_ctrl_cg_get()
{
    return hal_read32(&(pwm_ctrl->cg));
}


/*! Event_cfg. */
/* Output event source. */
static inline void hal_pwm_ctrl_evt_cfg_sel0_set(uint8_t output_evt)
{
    pwm_ctrl_event_cfg_set((pwm_ctrl_event_cfg_get() & ~PWM_CTRL_EVENT_CFG_SEL0_MASK)
                           | PWM_CTRL_EVENT_CFG_SEL0(output_evt));
}

static inline void hal_pwm_ctrl_evt_cfg_sel1_set(uint8_t output_evt)
{
    pwm_ctrl_event_cfg_set((pwm_ctrl_event_cfg_get() & ~PWM_CTRL_EVENT_CFG_SEL1_MASK)
                           | PWM_CTRL_EVENT_CFG_SEL1(output_evt));
}

static inline void hal_pwm_ctrl_evt_cfg_sel2_set(uint8_t output_evt)
{
    pwm_ctrl_event_cfg_set((pwm_ctrl_event_cfg_get() & ~PWM_CTRL_EVENT_CFG_SEL2_MASK)
                           | PWM_CTRL_EVENT_CFG_SEL2(output_evt));
}

static inline void hal_pwm_ctrl_evt_cfg_sel3_set(uint8_t output_evt)
{
    pwm_ctrl_event_cfg_set((pwm_ctrl_event_cfg_get() & ~PWM_CTRL_EVENT_CFG_SEL3_MASK)
                           | PWM_CTRL_EVENT_CFG_SEL3(output_evt));
}

/* Output event enable. */
static inline void hal_pwm_ctrl_evt_cfg_enable(uint8_t evt_sel)
{
    pwm_ctrl_event_cfg_set(pwm_ctrl_event_cfg_get() | PWM_CTRL_EVENT_CFG_ENA(0x1 << evt_sel));
}

/* Output event disable. */
static inline void hal_pwm_ctrl_evt_cfg_disable(uint8_t evt_sel)
{
    pwm_ctrl_event_cfg_set(pwm_ctrl_event_cfg_get() & ~PWM_CTRL_EVENT_CFG_ENA(0x1 << evt_sel));
}

/* Output event selection config. */
#define PI_HAL_PWM_EVENT_SEL0 ( 0x00 )
#define PI_HAL_PWM_EVENT_SEL1 ( 0x01 )
#define PI_HAL_PWM_EVENT_SEL2 ( 0x02 )
#define PI_HAL_PWM_EVENT_SEL3 ( 0x03 )
static inline void hal_pwm_ctrl_evt_cfg_set(uint8_t evt_sel, uint8_t output_evt)
{
    switch(evt_sel)
    {
    case PI_HAL_PWM_EVENT_SEL0 :
        hal_pwm_ctrl_evt_cfg_sel0_set(output_evt);
        break;
    case PI_HAL_PWM_EVENT_SEL1 :
        hal_pwm_ctrl_evt_cfg_sel1_set(output_evt);
        break;
    case PI_HAL_PWM_EVENT_SEL2 :
        hal_pwm_ctrl_evt_cfg_sel2_set(output_evt);
        break;
    case PI_HAL_PWM_EVENT_SEL3 :
        hal_pwm_ctrl_evt_cfg_sel3_set(output_evt);
        break;
    default :
        break;
    }
    hal_pwm_ctrl_evt_cfg_enable(evt_sel);
}


/*! CG. */
/* PWM timer enable. */
static inline void hal_pwm_ctrl_cg_disable(uint8_t pwm_id)
{
    pwm_ctrl_cg_set(pwm_ctrl_cg_get() | PWM_CTRL_CG_ENA(0x1 << pwm_id));
}

/* PWM timer disable. */
static inline void hal_pwm_ctrl_cg_enable(uint8_t pwm_id)
{
    pwm_ctrl_cg_set(pwm_ctrl_cg_get() & ~PWM_CTRL_CG_ENA(0x1 << pwm_id));
}

#endif  /* __PI_HAL_PWM_CTRL_H__ */
