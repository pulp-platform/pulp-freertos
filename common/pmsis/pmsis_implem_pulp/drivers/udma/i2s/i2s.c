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
 *****************************************************************************/

extern uint32_t g_i2s_flags;
extern struct i2s_driver_fifo_s *__global_i2s_driver_fifo[];

/*******************************************************************************
 * API implementation
 ******************************************************************************/

void pi_i2s_setup(uint32_t flags)
{
    g_i2s_flags = flags;
}

void pi_i2s_conf_init(struct pi_i2s_conf *conf)
{
    __pi_i2s_conf_init(conf);
}

int pi_i2s_open(struct pi_device *device)
{
    struct pi_i2s_conf *conf = (struct pi_i2s_conf *) device->config;
    if (__global_i2s_driver_fifo[conf->itf] == NULL)
    {
        __pi_i2s_open(conf);
        device->data = (void *) __global_i2s_driver_fifo[conf->itf];
    }
    else
    {
        __global_i2s_driver_fifo[conf->itf]->nb_open++;
    }
    return 0;
}

void pi_i2s_close(struct pi_device *device)
{
    struct i2s_driver_fifo_s *fifo = (struct i2s_driver_fifo_s *) device->data;
    fifo->nb_open--;
    if (fifo->nb_open == 0)
    {
        __pi_i2s_close(fifo->i2s_id);
    }
}

int pi_i2s_ioctl(struct pi_device *device, uint32_t cmd, void *arg)
{
    struct i2s_driver_fifo_s *fifo = (struct i2s_driver_fifo_s *) device->data;
    __pi_i2s_ioctl(fifo->i2s_id, cmd, arg);
    return 0;
}

int pi_i2s_read(struct pi_device *device, void **mem_block, size_t *size)
{
    pi_task_t task;
    pi_task_block(&task);
    pi_i2s_read_async(device, &task);
    pi_task_wait_on(&task);
    return pi_i2s_read_status(&task, mem_block, size);
}

int pi_i2s_read_async(struct pi_device *device, pi_task_t *task)
{
    struct i2s_driver_fifo_s *fifo = (struct i2s_driver_fifo_s *) device->data;
    __pi_i2s_read_async(fifo->i2s_id, task);
    return 0;
}

int pi_i2s_read_status(pi_task_t *task, void **mem_block, size_t *size)
{
    return __pi_i2s_read_status(task, mem_block, size);
}

int pi_i2s_write(struct pi_device *device, void *mem_block, size_t size)
{
    return -1;
}
