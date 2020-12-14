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
#include "pmsis/implem/drivers/fc_event/fc_event.h"
#include "pmsis/implem/drivers/udma/spi/spi_internal.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

#ifdef DEBUG
#define DEBUG_PRINTF printf
#else
#define DEBUG_PRINTF(...) ((void) 0)
#endif  /* DEBUG */

/*******************************************************************************
 * API implementation
 ******************************************************************************/

int pi_spi_open(struct pi_device *device)
{
    struct pi_spi_conf *conf = (struct pi_spi_conf *) device->config;
    int irq = __disable_irq();
    int status = __pi_spi_open((struct spim_cs_data**)(&device->data), conf);
    restore_irq(irq);
    return status;
}

void pi_spi_close(struct pi_device *device)
{
    struct pi_spi_conf *conf = (struct pi_spi_conf *) device->config;
    int irq = __disable_irq();
    __pi_spi_close(device->data);
    restore_irq(irq);
}

void pi_spi_ioctl(struct pi_device *device, uint32_t cmd, void *arg)
{
    // TODO
}

void pi_spi_send(struct pi_device *device, void *data, size_t len, pi_spi_flags_e flag)
{
    pi_task_t task_block;
    pi_task_block(&task_block);
    DEBUG_PRINTF("%s:%d\n",__func__,__LINE__);
    pi_spi_send_async(device, data, len, flag, &task_block);
    DEBUG_PRINTF("%s:%d\n",__func__,__LINE__);
    pi_task_wait_on(&task_block);
    DEBUG_PRINTF("%s:%d\n",__func__,__LINE__);
    pi_task_destroy(&task_block);
    DEBUG_PRINTF("%s:%d\n",__func__,__LINE__);
}

void pi_spi_send_async(struct pi_device *device, void *data, size_t len, pi_spi_flags_e flag, pi_task_t *task)
{
    __pi_spi_send_async(device->data, data, len, flag, task);
}

void pi_spi_receive(struct pi_device *device, void *data, size_t len, pi_spi_flags_e flag)
{
    pi_task_t task_block;
    pi_task_block(&task_block);
    DEBUG_PRINTF("%s:%d\n",__func__,__LINE__);
    pi_spi_receive_async(device, data, len, flag, &task_block);
    DEBUG_PRINTF("%s:%d\n",__func__,__LINE__);
    pi_task_wait_on(&task_block);
    DEBUG_PRINTF("%s:%d\n",__func__,__LINE__);
    pi_task_destroy(&task_block);
    DEBUG_PRINTF("%s:%d\n",__func__,__LINE__);
}

void pi_spi_receive_async(struct pi_device *device, void *data, size_t len, pi_spi_flags_e flag, pi_task_t *task)
{
    __pi_spi_receive_async(device->data, data, len, flag, task);
}

void pi_spi_receive_with_ucode(struct pi_device *device, void *data, size_t len,
        pi_spi_flags_e flags, int ucode_size, void *ucode)
{
    pi_task_t task_block;
    pi_task_block(&task_block);
    __pi_spi_receive_async_with_ucode(device->data, data, len, flags,
            ucode_size, ucode, &task_block);
    pi_task_wait_on(&task_block);
    pi_task_destroy(&task_block);
}

void pi_spi_send_with_ucode(struct pi_device *device, void *data, size_t len,
        pi_spi_flags_e flags, int ucode_size, void *ucode)
{
    pi_task_t task_block;
    pi_task_block(&task_block);
    __pi_spi_send_async_with_ucode(device->data, data, len, flags,
            ucode_size, ucode, &task_block);
    pi_task_wait_on(&task_block);
    pi_task_destroy(&task_block);
}

uint32_t pi_spi_get_config(struct pi_device *device)
{
    return __pi_spi_get_config(device->data);
}

void pi_spi_transfer(struct pi_device *device, void *tx_data, void *rx_data, size_t len, pi_spi_flags_e flag)
{
    // TODO
    pi_task_t task_block;
    pi_task_block(&task_block);
    DEBUG_PRINTF("%s:%d\n",__func__,__LINE__);
    pi_spi_transfer_async(device, tx_data, rx_data, len, flag, &task_block);
    DEBUG_PRINTF("%s:%d\n",__func__,__LINE__);
    pi_task_wait_on(&task_block);
    DEBUG_PRINTF("%s:%d\n",__func__,__LINE__);
    pi_task_destroy(&task_block);
    DEBUG_PRINTF("%s:%d\n",__func__,__LINE__);
}

void pi_spi_transfer_async(struct pi_device *device, void *tx_data, void *rx_data, size_t len, pi_spi_flags_e flag, pi_task_t *task)
{
    // TODO
    __pi_spi_xfer_async(device->data, tx_data, rx_data, len, flag, task);
}
