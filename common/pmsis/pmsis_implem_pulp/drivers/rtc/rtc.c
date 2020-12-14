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

#include "pmsis/implem/drivers/rtc/rtc_internal.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Driver data
 ******************************************************************************/

/*******************************************************************************
 * API implementation
 ******************************************************************************/

void pi_rtc_conf_init(struct pi_rtc_conf *conf)
{
    __pi_rtc_conf_init(conf);
}

int pi_rtc_open(struct pi_device *device)
{
    struct pi_rtc_conf *conf = (struct pi_rtc_conf *) device->config;
    device->data = (void *) ((uint32_t) conf->rtc_id);
    return __pi_rtc_open(conf->rtc_id, conf);
}

void pi_rtc_close(struct pi_device *device)
{
    __pi_rtc_close((uint32_t) device->data);
}

int pi_rtc_datetime_set(struct pi_device *device, struct tm *time)
{
    return __pi_rtc_datetime_set((uint32_t) device->data, time);
}

int pi_rtc_datetime_get(struct pi_device *device, struct tm *time)
{
    return __pi_rtc_datetime_get((uint32_t) device->data, time);
}

int pi_rtc_alarm_set(struct pi_device *device, struct tm *alarm)
{
    return __pi_rtc_alarm_set((uint32_t) device->data, alarm);
}

int pi_rtc_alarm_get(struct pi_device *device, struct tm *alarm)
{
    return __pi_rtc_alarm_get((uint32_t) device->data, alarm);
}

int pi_rtc_timer_set(struct pi_device *device, uint32_t countdown)
{
    return __pi_rtc_timer_set((uint32_t) device->data, countdown);
}

uint32_t pi_rtc_timer_get(struct pi_device *device)
{
    return __pi_rtc_timer_get((uint32_t) device->data);
}

int pi_rtc_ioctl(struct pi_device *device, uint32_t cmd, void *arg)
{
    return __pi_rtc_ioctl((uint32_t) device->data, cmd, arg);
}
