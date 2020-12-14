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

#ifndef __PI_HAL_PWM_H__
#define __PI_HAL_PWM_H__

#include "pmsis/targets/target.h"

/* Cmd register. */
static inline void pwm_cmd_set(uint8_t pwm_id, uint32_t cmd)
{
    hal_write32(&(pwm(pwm_id)->cmd), cmd);
}

static inline uint32_t pwm_cmd_get(uint8_t pwm_id)
{
    return hal_read32(&(pwm(pwm_id)->cmd));
}


/* Config register. */
static inline void pwm_config_set(uint8_t pwm_id, uint32_t config)
{
    hal_write32(&(pwm(pwm_id)->config), config);
}

static inline uint32_t pwm_config_get(uint8_t pwm_id)
{
    return hal_read32(&(pwm(pwm_id)->config));
}


/* Threshold register. */
static inline void pwm_threshold_set(uint8_t pwm_id, uint32_t threshold)
{
    hal_write32(&(pwm(pwm_id)->threshold), threshold);
}

static inline uint32_t pwm_threshold_get(uint8_t pwm_id)
{
    return hal_read32(&(pwm(pwm_id)->threshold));
}


/* Ch_Threshold register. */
static inline void pwm_ch_threshold_set(uint8_t pwm_id, uint8_t ch_id, uint32_t ch_threshold)
{
    hal_write32(&(pwm(pwm_id)->ch_threshold[ch_id]), ch_threshold);
}

static inline uint32_t pwm_ch_threshold_get(uint8_t pwm_id, uint8_t ch_id)
{
    return hal_read32(&(pwm(pwm_id)->ch_threshold[ch_id]));
}


/* Counter register. */
static inline uint32_t pwm_counter_get(uint8_t pwm_id)
{
    return hal_read32(&(pwm(pwm_id)->counter));
}


/*! Cmd. */
/* Cmd setting. */
static inline void hal_pwm_cmd_set(uint8_t pwm_id, uint16_t cmd)
{
    pwm_cmd_set(pwm_id, cmd);
}


/*! Config. */
/* Input source selection. */
static inline void hal_pwm_config_insel_set(uint8_t pwm_id, uint8_t insel)
{
    pwm_config_set(pwm_id, (pwm_config_get(pwm_id) & ~PWM_CONFIG_INSEL_MASK)
                           | PWM_CONFIG_INSEL(insel));
}

/* Trigger mode config. */
static inline void hal_pwm_config_mode_set(uint8_t pwm_id, uint8_t mode)
{
    pwm_config_set(pwm_id, (pwm_config_get(pwm_id) & ~PWM_CONFIG_MODE_MASK)
                           | PWM_CONFIG_MODE(mode));
}

/* Clock source selection. */
static inline void hal_pwm_config_clksel_set(uint8_t pwm_id, uint8_t clksel)
{
    pwm_config_set(pwm_id, (pwm_config_get(pwm_id) & ~PWM_CONFIG_CLKSEL_MASK)
                           | PWM_CONFIG_CLKSEL(clksel));
}

/* Counter counting method. */
static inline void hal_pwm_config_updown_set(uint8_t pwm_id, uint8_t updown)
{
    pwm_config_set(pwm_id, (pwm_config_get(pwm_id) & ~PWM_CONFIG_UPDOWNSEL_MASK)
                           | PWM_CONFIG_UPDOWNSEL(updown));
}

/* Prescaler value. */
static inline void hal_pwm_config_presc_set(uint8_t pwm_id, uint8_t presc)
{
    pwm_config_set(pwm_id, (pwm_config_get(pwm_id) & ~PWM_CONFIG_PRESC_MASK)
                           | PWM_CONFIG_PRESC(presc));
}

/*
 * Config setting : input source selection, trigger mode, clock source selection,
 * counting method and prescaler value.
 */
static inline void hal_pwm_config_set(uint8_t pwm_id, uint8_t insel, uint8_t mode,
                                      uint8_t clksel, uint8_t updown, uint8_t prescaler)
{
    uint32_t config = PWM_CONFIG_INSEL(insel) |
                      PWM_CONFIG_MODE(mode) |
                      PWM_CONFIG_CLKSEL(clksel) |
                      PWM_CONFIG_UPDOWNSEL(mode) |
                      PWM_CONFIG_PRESC(prescaler);
    pwm_config_set(pwm_id, config);
}

static inline void hal_pwm_config_reg_set(uint8_t pwm_id, uint32_t config)
{
    pwm_config_set(pwm_id, config);
}


/*! Threshold. */
/* Threshold setting. */
static inline void hal_pwm_threshold_set(uint8_t pwm_id, uint16_t th_lo,
                                         uint16_t th_hi)
{
    pwm_threshold_set(pwm_id, PWM_THRESHOLD_TH_HI(th_hi) |
                              PWM_THRESHOLD_TH_LO(th_lo));
}


/*! Ch_threshold. */
/* Channel threshold setting. */
static inline void hal_pwm_channel_th_set(uint8_t pwm_id, uint8_t ch_id,
                                          uint16_t threshold)
{
    pwm_ch_threshold_set(pwm_id, ch_id, (pwm_ch_threshold_get(pwm_id, ch_id)
                                         & ~PWM_CH_THRESHOLD_TH_MASK)
                                         | PWM_CH_THRESHOLD_TH(threshold));
}

/* Channel mode setting. */
static inline void hal_pwm_channel_mode_set(uint8_t pwm_id, uint8_t ch_id,
                                            uint8_t mode)
{
    pwm_ch_threshold_set(pwm_id, ch_id, (pwm_ch_threshold_get(pwm_id, ch_id)
                                         & ~PWM_CH_THRESHOLD_MODE_MASK)
                                         | PWM_CH_THRESHOLD_MODE(mode));
}

/* Channel setting. */
static inline void hal_pwm_channel_th_mode_set(uint8_t pwm_id, uint8_t ch_id,
                                              uint16_t threshold, uint8_t mode)
{
    uint32_t cfg = PWM_CH_THRESHOLD_TH(threshold) | PWM_CH_THRESHOLD_MODE(mode);
    pwm_ch_threshold_set(pwm_id, ch_id, cfg);
}


/*! Counter. */
/* Counter value. */
static inline uint32_t hal_pwm_counter_get(uint8_t pwm_id)
{
    return pwm_counter_get(pwm_id);
}

#endif  /* __PI_HAL_PWM_H__ */
