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
#include "pmsis/implem/drivers/udma/cpi/cpi_internal.h"
#include "pmsis/implem/drivers/fc_event/fc_event.h"
#include "pmsis/implem/hal/hal.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Driver data
 *****************************************************************************/

/*******************************************************************************
 * API implementation
 ******************************************************************************/

void pi_cpi_conf_init(struct pi_cpi_conf *conf)
{
    __pi_cpi_conf_init(conf);
}

int pi_cpi_open(struct pi_device *device)
{
    int32_t status = -1;
    struct pi_cpi_conf *conf = (struct pi_cpi_conf *) device->config;
    CPI_TRACE("Open device id=%ld\n", conf->itf);
    status = __pi_cpi_open(conf, (struct cpi_itf_data_s **) &(device->data));
    CPI_TRACE("Open status : %ld, driver data : %lx\n",
              status, (struct cpi_itf_data_s *) device->data);
    return status;
}

void pi_cpi_close(struct pi_device *device)
{
    struct cpi_itf_data_s *device_data =  (struct cpi_itf_data_s *) device->data;
    CPI_TRACE("Close device id=%ld\n", device_data->device_id);
    if (device_data != NULL)
    {
        __pi_cpi_close(device_data);
        device->data = NULL;
    }
}

#define MUTEX 1
void pi_cpi_capture(struct pi_device *device, void *buffer, int32_t bufferlen)
{
    pi_task_t task_block;
    #if MUTEX
    pi_task_block(&task_block);
    pi_cpi_capture_async(device, buffer, bufferlen, &task_block);
    pi_task_wait_on(&task_block);
    pi_task_destroy(&task_block);
    #else
    pi_task_block_no_mutex(&task_block);
    pi_cpi_capture_async(device, buffer, bufferlen, &task_block);
    pi_task_wait_on_no_mutex(&task_block);
    #endif
}

void pi_cpi_capture_async(struct pi_device *device, void *buffer,
                          int32_t bufferlen, struct pi_task *task)
{
    struct cpi_itf_data_s *device_data = (struct cpi_itf_data_s *) device->data;
    CPI_TRACE("CPI(%d) RX Transfer : %lx %ld\n",
              device_data->device_id, buffer, bufferlen);
    __pi_cpi_copy(device_data, buffer, bufferlen, task);
}
