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

#ifndef __PWM_INTERNAL_H__
#define __PWM_INTERNAL_H__

#include "pmsis.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Driver data
 ******************************************************************************/

/*******************************************************************************
 * Function declaration
 ******************************************************************************/

void __pi_pwm_conf_init(struct pi_pwm_conf *conf);

int32_t __pi_pwm_open(uint8_t pwm_id, struct pi_pwm_conf *conf);

void __pi_pwm_close(uint8_t pwm_id);

int32_t __pi_pwm_ioctl(uint8_t pwm_id, pi_pwm_ioctl_cmd_e cmd, void *arg);

uint32_t __pi_pwm_counter_get(uint8_t pwm_id);


/*******************************************************************************
 * API implementation
 ******************************************************************************/

static inline void pi_pwm_timer_start(struct pi_device *device)
{
    uint8_t pwm_id = ((uint32_t) device->data) & 0xFF;
    __pi_pwm_ioctl(pwm_id, PI_PWM_TIMER_COMMAND, (void *) PI_PWM_CMD_START);
}

static inline void pi_pwm_timer_stop(struct pi_device *device)
{
    uint8_t pwm_id = ((uint32_t) device->data) & 0xFF;
    __pi_pwm_ioctl(pwm_id, PI_PWM_TIMER_COMMAND, (void *) PI_PWM_CMD_STOP);
}

#endif  /* __PWM_INTERNAL_H__ */
