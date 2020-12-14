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

extern struct dmacpy_driver_fifo_s *g_dmacpy_driver_fifo[];

/*******************************************************************************
 * API implementation
 ******************************************************************************/

void pi_dmacpy_conf_init(struct pi_dmacpy_conf *conf)
{
    __pi_dmacpy_conf_init(conf);
}

int pi_dmacpy_open(struct pi_device *device)
{
    int32_t status = -1;
    struct pi_dmacpy_conf *conf = (struct pi_dmacpy_conf *) device->config;
    DMACPY_TRACE("Open device id=%d\n", conf->id);
    status = __pi_dmacpy_open(conf, (struct dmacpy_driver_fifo_s **) &(device->data));
    DMACPY_TRACE("Open status : %d\n", status);
    return status;
}

void pi_dmacpy_close(struct pi_device *device)
{
    struct dmacpy_driver_fifo_s *fifo = (struct dmacpy_driver_fifo_s *) device->data;
    if (fifo != NULL)
    {
        DMACPY_TRACE("Close device id=%d\n", fifo->device_id);
        __pi_dmacpy_close(fifo->device_id);
        device->data = NULL;
    }
}

int pi_dmacpy_copy(struct pi_device *device, void *src, void *dst,
                   uint32_t size, pi_dmacpy_dir_e dir)
{
    int status = 0;
    pi_task_t task = {0};
    pi_task_block(&task);
    status = pi_dmacpy_copy_async(device, src, dst, size, dir, &task);
    if (status != 0)
    {
        DMACPY_TRACE_ERR("Error on copy %d!\n", status);
        pi_task_destroy(&task);
        return status;
    }
    pi_task_wait_on(&task);
    pi_task_destroy(&task);
    return status;
}

int pi_dmacpy_copy_async(struct pi_device *device, void *src, void *dst,
                         uint32_t size, pi_dmacpy_dir_e dir, struct pi_task *task)
{
    struct dmacpy_driver_fifo_s *fifo = (struct dmacpy_driver_fifo_s *) device->data;
    DMACPY_TRACE("DMA Memcpy(%ld): %lx %lx %ld %ld\n",
                 fifo->device_id, (uint32_t) src, (uint32_t) dst, size, dir);
    return __pi_dmacpy_copy(fifo->device_id, src, dst, size, dir, task);
}
