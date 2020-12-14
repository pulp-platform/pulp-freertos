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

#ifndef __PI_HAL_UDMA_I2C_H__
#define __PI_HAL_UDMA_I2C_H__

#include "pmsis/targets/target.h"
#include "pmsis/implem/hal/udma/udma_core.h"
#include "pmsis/implem/hal/udma/udma_ctrl.h"

/* I2C udma configuration. */
static inline void i2c_udma_channel_set(uint32_t device_id, udma_channel_e channel,
                                        uint32_t l2buf, uint32_t size, uint32_t cfg)
{
	switch(channel)
	{
        case(RX_CHANNEL):
		udma_enqueue_channel(&(i2c(device_id)->rx), l2buf, size, cfg);
		break;
        case(TX_CHANNEL):
		udma_enqueue_channel(&(i2c(device_id)->tx), l2buf, size, cfg);
		break;
        case(COMMAND_CHANNEL):
		udma_enqueue_channel(&(i2c(device_id)->cmd), l2buf, size, cfg);
		break;
        default:
		break;
	}
}


/* Status register. */
static inline void i2c_status_set(uint32_t device_id, uint32_t status)
{
    hal_write32(&(i2c(device_id)->status), status);
}

static inline uint32_t i2c_status_get(uint32_t device_id)
{
    return hal_read32(&(i2c(device_id)->status));
}


/* Setup register. */
static inline void i2c_setup_set(uint32_t device_id, uint32_t setup)
{
    hal_write32(&(i2c(device_id)->setup), setup);
}

static inline uint32_t i2c_setup_get(uint32_t device_id)
{
    return hal_read32(&(i2c(device_id)->setup));
}


/*! STATUS. */
/* i2c busy check. */
static inline uint32_t i2c_busy_get(uint32_t device_id)
{
    return i2c_status_get(device_id) & ~I2C_STATUS_BUSY_MASK;
}

/* Enable arbitration lost error. */
static inline void i2c_arbitration_set(uint32_t device_id, uint8_t value)
{
    i2c_status_set(device_id,
                   (i2c_status_get(device_id) & ~I2C_STATUS_ARB_LOST_MASK)
                   | I2C_STATUS_ARB_LOST(value));
}

static inline uint32_t i2c_arbitration_get(uint32_t device_id)
{
    return i2c_status_get(device_id) & ~I2C_STATUS_ARB_LOST_MASK;
}


/*! SETUP. */
/* Abort/Reset on-going transfer & clear busy & arbitration flags. */
static inline void i2c_do_reset_set(uint32_t device_id, uint8_t value)
{
    i2c_setup_set(device_id,
                  (i2c_setup_get(device_id) & ~I2C_SETUP_DO_RST_MASK)
                  | I2C_SETUP_DO_RST(value));
}

static inline uint32_t i2c_do_reset_get(uint32_t device_id)
{
    return i2c_setup_get(device_id) & ~I2C_SETUP_DO_RST_MASK;
}


/*! UDMA.  */
/* Enqueue transfer on UDMA channels. */
static inline void hal_i2c_enqueue(uint32_t device_id, uint32_t channel,
                                   uint32_t l2buf, uint32_t size, uint32_t cfg)
{
    /* Enqueue l2 buffer & start transfer. */
    i2c_udma_channel_set(device_id, channel, l2buf, size, cfg);
}

#endif  /* __PI_HAL_UDMA_I2C_H__ */
