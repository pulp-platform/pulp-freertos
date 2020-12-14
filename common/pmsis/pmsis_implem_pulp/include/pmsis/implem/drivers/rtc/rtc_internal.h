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

#ifndef __PI_DRIVER_RTC_INTERNAL_H__
#define __PI_DRIVER_RTC_INTERNAL_H__

#include "pmsis/drivers/rtc.h"
#include "pmsis/targets/target.h"
#include "pmsis/implem/hal/hal.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

#define PI_RTC_ATTACH_CB_CAL       ( 8 )
#define PI_RTC_ATTACH_CB_ALARM     ( 9 )
#define PI_RTC_ATTACH_CB_TIMER     ( 10 )

/*******************************************************************************
 * Driver data
 ******************************************************************************/

/*******************************************************************************
 * Function declaration
 ******************************************************************************/

void __pi_rtc_conf_init(struct pi_rtc_conf *conf);

int32_t __pi_rtc_open(uint8_t rtc_id, struct pi_rtc_conf *conf);

void __pi_rtc_close(uint8_t rtc_id);

int32_t __pi_rtc_datetime_set(uint8_t rtc_id, struct tm *time);

int32_t __pi_rtc_datetime_get(uint8_t rtc_id, struct tm *time);

int32_t __pi_rtc_alarm_set(uint8_t rtc_id, struct tm *alarm);

int32_t __pi_rtc_alarm_get(uint8_t rtc_id, struct tm *alarm);

int32_t __pi_rtc_timer_set(uint8_t rtc_id, uint32_t countdown);

uint32_t __pi_rtc_timer_get(uint8_t rtc_id);

int32_t __pi_rtc_ioctl(uint8_t rtc_id, uint32_t cmd, void *arg);

#endif  /* __PI_DRIVER_RTC_INTERNAL_H__ */
