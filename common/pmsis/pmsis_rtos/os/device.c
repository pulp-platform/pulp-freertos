/*
 * Copyright (c) 2018, GreenWaves Technologies, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of GreenWaves Technologies, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "pmsis.h"
#include "pmsis/device.h"
#include "pmsis/rtos/rtos.h"

static inline pi_device_e get_pi_device_type(void *conf);

// extract pi_device_type
static inline pi_device_e get_pi_device_type(void *conf)
{
    pi_device_e *device_ptr = (pi_device_e*)conf;
    return *device_ptr;
}

void pi_open_from_conf(struct pi_device *device, void *conf)
{
    #if 0
    pi_device_e device_type = get_pi_device_type(conf);
    switch (device_type)
    {
    case PI_DEVICE_CLUSTER_TYPE:
        device->config = conf;
        pi_cluster_open(device);
        break;
    case PI_DEVICE_CPI_TYPE:
    case PI_DEVICE_HYPERBUS_TYPE:
    case PI_DEVICE_I2C_TYPE:
    case PI_DEVICE_SPI_TYPE:
    case PI_DEVICE_GPIO_TYPE:
        device->config = conf;
        break;
    default:
        device->config = conf;
        // TODO: add debug warning here
        break;
    }
    #else
    device->config = conf;
    #endif
}
