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

#include "pmsis/implem/drivers/udma/uart/uart_internal.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Driver data
 ******************************************************************************/

/*******************************************************************************
 * API implementation
 ******************************************************************************/

void pi_uart_conf_init(struct pi_uart_conf *conf)
{
    __pi_uart_conf_init(conf);
}

int pi_uart_open(struct pi_device *device)
{
    int32_t status = 1;
    struct pi_uart_conf *conf = (struct pi_uart_conf *) device->config;
    UART_TRACE("Open device id=%ld\n", conf->uart_id);
    status = __pi_uart_open((struct uart_itf_data_s **) &(device->data), conf);
    UART_TRACE("Open status : %ld\n", status);
    return status;
}

void pi_uart_close(struct pi_device *device)
{
    struct uart_itf_data_s *data = (struct uart_itf_data_s *) device->data;
    if (data != NULL)
    {
        UART_TRACE("Close device id=%d\n", data->device_id);
        __pi_uart_close(data);
        device->data = NULL;
    }
}

int pi_uart_ioctl(struct pi_device *device, uint32_t cmd, void *arg)
{
    struct uart_itf_data_s *data = (struct uart_itf_data_s *) device->data;
    UART_TRACE("Ioctl device id=%ld cmd=%ld arg=%lx\n",
               data->device_id, cmd, arg);
    return __pi_uart_ioctl(data, cmd, arg);
}

int pi_uart_write(struct pi_device *device, void *buffer, uint32_t size)
{
    int32_t status = 0;
    pi_task_t task_block = {0};
    pi_task_block(&task_block);
    status = pi_uart_write_async(device, buffer, size, &task_block);
    pi_task_wait_on(&task_block);
    pi_task_destroy(&task_block);
    return status;
}

int pi_uart_write_byte(struct pi_device *device, uint8_t *byte)
{
    int32_t status = 0;
    pi_task_t task_block = {0};
    pi_task_block(&task_block);
    status = pi_uart_write_async(device, byte, 1, &task_block);
    pi_task_wait_on(&task_block);
    pi_task_destroy(&task_block);
    return status;
}

int pi_uart_write_byte_async(struct pi_device *device, uint8_t *byte,
                             pi_task_t* callback)
{
    return pi_uart_write_async(device, byte, 1, callback);
}

int pi_uart_write_async(struct pi_device *device, void *buffer, uint32_t size,
                        pi_task_t* callback)
{
    struct uart_itf_data_s *data = (struct uart_itf_data_s *) device->data;
    uint32_t l2_buf = (uint32_t) buffer;
    UART_TRACE("UART(%ld): Write l2_buf=%lx size=%ld\n",
               data->device_id, l2_buf, size);
    return __pi_uart_copy(data, l2_buf, size, TX_CHANNEL, callback);
}

int pi_uart_read(struct pi_device *device, void *buffer, uint32_t size)
{
    int32_t status = 0;
    pi_task_t task_block = {0};
    pi_task_block(&task_block);
    status = pi_uart_read_async(device, buffer, size, &task_block);
    pi_task_wait_on(&task_block);
    pi_task_destroy(&task_block);
    return status;
}

int pi_uart_read_byte(struct pi_device *device, uint8_t *byte)
{
    int32_t status = 0;
    pi_task_t task_block = {0};
    pi_task_block(&task_block);
    status = pi_uart_read_async(device, byte, 1, &task_block);
    pi_task_wait_on(&task_block);
    pi_task_destroy(&task_block);
    return status;
}

int pi_uart_read_async(struct pi_device *device, void *buffer, uint32_t size,
                       pi_task_t* callback)
{
    struct uart_itf_data_s *data = (struct uart_itf_data_s *) device->data;
    uint32_t l2_buf = (uint32_t) buffer;
    UART_TRACE("UART(%ld): Read l2_buf=%lx size=%ld\n",
               data->device_id, l2_buf, size);
    return __pi_uart_copy(data, l2_buf, size, RX_CHANNEL, callback);
}
