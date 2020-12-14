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

#include "pmsis.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Driver data
 ******************************************************************************/

static pi_task_t *__cb_task_array[ARCHI_NB_PWM] = {NULL};

/*******************************************************************************
 * Internal functions
 ******************************************************************************/

static void __pi_pwm_timer_conf_set(uint8_t pwm_id, struct pi_pwm_conf *conf)
{
    uint32_t config = (uint32_t) conf->timer_conf;
    config |= (conf->input_src << PI_PWM_CONFIG_INSEL_OFFSET);
    config |= (conf->prescaler << PI_PWM_CONFIG_PRESCALER_OFFSET);
    hal_pwm_config_reg_set(pwm_id, config);
}

static void __pi_pwm_threshold_set(uint8_t pwm_id, uint16_t counter_start,
                                   uint16_t counter_end)
{
    hal_pwm_threshold_set(pwm_id, counter_start, counter_end);
}

static void __pi_pwm_channel_config_set(uint8_t pwm_id, pi_pwm_channel_e channel,
                                        uint16_t threshold, pi_pwm_ch_config_e config)
{
    hal_pwm_channel_th_mode_set(pwm_id, channel, threshold, config);
}

static void __pi_pwm_output_event_set(pi_pwm_evt_sel_e evt_sel, pi_pwm_output_evt_e evt_output)
{
    hal_pwm_ctrl_evt_cfg_set(evt_sel, evt_output);
}

static void __pi_pwm_output_event_clear(pi_pwm_evt_sel_e evt_sel)
{
    hal_pwm_ctrl_evt_cfg_disable(evt_sel);
}

static void __pi_pwm_user_cb_attach(uint8_t pwm_id, pi_task_t *cb)
{
    __cb_task_array[pwm_id] = cb;
}

static void __pi_pwm_command_set(uint8_t pwm_id, pi_pwm_cmd_e cmd)
{
    hal_pwm_cmd_set(pwm_id, cmd);
}

/*******************************************************************************
 * API implementation
 ******************************************************************************/

void pwm_handler(void *arg)
{
    uint32_t event = (uint32_t) arg;
    uint32_t periph_id = event - SOC_EVENT_PWM(0);

    if (__cb_task_array[periph_id] != NULL)
    {
        pmsis_event_push(pmsis_event_get_default_scheduler(), __cb_task_array[periph_id]);
    }
}

void __pi_pwm_conf_init(struct pi_pwm_conf *conf)
{
    conf->device = PI_DEVICE_PWM_TYPE;
    conf->pwm_id = 0;
    conf->ch_id = PI_PWM_CHANNEL0;
    conf->input_src = 0;
    conf->timer_conf = PI_PWM_EVT_EACH_CLK_CYCLE |
                       PI_PWM_CLKSEL_REFCLK_32K |
                       PI_PWM_UPDOWNSEL_RESET;
    conf->prescaler = 0;
}

int32_t __pi_pwm_open(uint8_t pwm_id, struct pi_pwm_conf *conf)
{
    /* Set handler. */
    pi_fc_event_handler_set(SOC_EVENT_PWM(pwm_id), pwm_handler);
    /* Enable SOC event propagation to FC. */
    hal_soc_eu_set_fc_mask(SOC_EVENT_PWM(pwm_id));

    /* Disable PWM CG. */
    hal_pwm_ctrl_cg_disable(pwm_id);

    /* Setup PWM timer. */
    __pi_pwm_timer_conf_set(pwm_id, conf);

    return 0;
}

void __pi_pwm_close(uint8_t pwm_id)
{
    /* Clear handler. */
    pi_fc_event_handler_clear(SOC_EVENT_PWM(pwm_id));
    /* Disable SOC event propagation to FC. */
    hal_soc_eu_clear_fc_mask(SOC_EVENT_PWM(pwm_id));

    /* Enable PWM CG. */
    hal_pwm_ctrl_cg_enable(pwm_id);

    /* Clear any attached callback. */
    __cb_task_array[pwm_id] = NULL;
}

int32_t __pi_pwm_ioctl(uint8_t pwm_id, pi_pwm_ioctl_cmd_e cmd, void *arg)
{
    pi_pwm_cmd_e timer_cmd = (pi_pwm_cmd_e) arg;
    struct pi_pwm_conf *conf = (struct pi_pwm_conf *) arg;
    uint32_t threshold = (uint32_t) arg;
    struct pi_pwm_ioctl_ch_config *ch_conf = (struct pi_pwm_ioctl_ch_config *) arg;
    struct pi_pwm_ioctl_evt *evt = (struct pi_pwm_ioctl_evt *) arg;
    pi_task_t *cb_task = (pi_task_t *) arg;

    switch (cmd)
    {
    case PI_PWM_TIMER_COMMAND :
        __pi_pwm_command_set(pwm_id, timer_cmd);
        return 0;

    case PI_PWM_TIMER_CONFIG :
        __pi_pwm_timer_conf_set(pwm_id, conf);
        return 0;

    case PI_PWM_TIMER_THRESH :
        __pi_pwm_threshold_set(pwm_id, threshold & 0xFFFF, threshold >> 16);
        return 0;

    case PI_PWM_CH_CONFIG :
        __pi_pwm_channel_config_set(pwm_id, ch_conf->channel, ch_conf->ch_threshold,
                                    ch_conf->config);
        return 0;

    case PI_PWM_EVENT_SET :
        __pi_pwm_output_event_set(evt->evt_sel, evt->evt_output);
        return 0;

    case PI_PWM_EVENT_CLEAR :
        __pi_pwm_output_event_clear(evt->evt_sel);
        return 0;

    case PI_PWM_ATTACH_CB :
        __pi_pwm_user_cb_attach(pwm_id, cb_task);
    default :
        return -1;
    }
}

uint32_t __pi_pwm_counter_get(uint8_t pwm_id)
{
    return hal_pwm_counter_get(pwm_id);
}
