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

#ifndef __PI_HAL_RTC_H__
#define __PI_HAL_RTC_H__

#include "pmsis/targets/target.h"

/* APB Status Register. */
static inline void rtc_apb_sr_set(uint8_t rtc_id, uint32_t value)
{
    hal_write32(&(rtc(rtc_id)->apb_sr), value);
}

static inline uint32_t rtc_apb_sr_get(uint8_t rtc_id)
{
    return hal_read32(&(rtc(rtc_id)->apb_sr));
}

/* APB Control Register. */
static inline void rtc_apb_cr_set(uint8_t rtc_id, uint32_t value)
{
    hal_write32(&(rtc(rtc_id)->apb_cr), value);
}

static inline uint32_t rtc_apb_cr_get(uint8_t rtc_id)
{
    return hal_read32(&(rtc(rtc_id)->apb_cr));
}

/* APB Data Register. */
static inline void rtc_apb_dr_set(uint8_t rtc_id, uint32_t value)
{
    hal_write32(&(rtc(rtc_id)->apb_dr), value);
}

static inline uint32_t rtc_apb_dr_get(uint8_t rtc_id)
{
    return hal_read32(&(rtc(rtc_id)->apb_dr));
}

/* APB IRQ Control Register. */
static inline void rtc_apb_icr_set(uint8_t rtc_id, uint32_t value)
{
    hal_write32(&(rtc(rtc_id)->apb_icr), value);
}

static inline uint32_t rtc_apb_icr_get(uint8_t rtc_id)
{
    return hal_read32(&(rtc(rtc_id)->apb_icr));
}

/* APB IRQ Mask Register. */
static inline void rtc_apb_imr_set(uint8_t rtc_id, uint32_t value)
{
    hal_write32(&(rtc(rtc_id)->apb_imr), value);
}

static inline uint32_t rtc_apb_imr_get(uint8_t rtc_id)
{
    return hal_read32(&(rtc(rtc_id)->apb_imr));
}

/* APB IRQ Flag Register. */
static inline void rtc_apb_ifr_set(uint8_t rtc_id, uint32_t value)
{
    hal_write32(&(rtc(rtc_id)->apb_ifr), value);
}

static inline uint32_t rtc_apb_ifr_get(uint8_t rtc_id)
{
    return hal_read32(&(rtc(rtc_id)->apb_ifr));
}


/*! APB Status Register. */
static inline uint32_t hal_rtc_status_get(uint8_t rtc_id)
{
    return rtc_apb_sr_get(rtc_id);
}

/*! APB Control Register. */
static inline void hal_rtc_request_set(uint8_t rtc_id, uint8_t addr, uint8_t write)
{
    rtc_apb_cr_set(rtc_id, RTC_APB_CR_APB_ADDR(addr) | RTC_APB_CR_APB_OP(write));
}


/*! APB Data Register. */
static inline void hal_rtc_data_set(uint8_t rtc_id, uint32_t data)
{
    rtc_apb_dr_set(rtc_id, data);
}

static inline uint32_t hal_rtc_data_get(uint8_t rtc_id)
{
    return rtc_apb_dr_get(rtc_id);
}


/*! APB IRQ Flag Register. */
static inline void hal_rtc_irq_flag_read_reset(uint8_t rtc_id)
{
    rtc_apb_ifr_set(rtc_id, (rtc_apb_ifr_get(rtc_id)
                             & ~RTC_APB_IFR_READ_FLAG_MASK)
                             | RTC_APB_IFR_READ_FLAG(1));
}

static inline void hal_rtc_irq_flag_write_reset(uint8_t rtc_id)
{
    rtc_apb_ifr_set(rtc_id, (rtc_apb_ifr_get(rtc_id)
                             & ~RTC_APB_IFR_WRITE_FLAG_MASK)
                             | RTC_APB_IFR_WRITE_FLAG(1));
}

#endif  /* __PI_HAL_RTC_H__ */
