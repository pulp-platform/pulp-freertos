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

#if (FEATURE_CLUSTER == 1)

#include "pmsis.h"
#include "pmsis/cluster/cluster_sync/cl_to_fc_delegate.h"
#include "pmsis/implem/cluster/delegate/hyperbus/hyperbus_cl_internal.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Driver data
 *****************************************************************************/

static pi_task_t task_delegate_exec;   /*!< pi_task used to keep info of current delegate. */
static pi_cl_hyper_req_t *fifo = NULL; /*!< FIFO of hyper req. */

/*******************************************************************************
 * Function declaration
 ******************************************************************************/

/* Signal a delegate is done, it wakes up cluster. */
void __pi_cl_hyper_copy_task_done(void *arg);

/*******************************************************************************
 * Function implementation.
 ******************************************************************************/

static void __pi_cl_delegate_hyper_copy_exec(void *arg)
{
    pi_cl_hyper_req_t *req = (pi_cl_hyper_req_t *) arg;
    struct pi_hyper_conf *conf = req->device->config;
    pi_task_callback_no_mutex(&task_delegate_exec, __pi_cl_hyper_copy_task_done, (void *) req);
    if (req->is_2d)
    {
        __pi_hyper_copy_2d(conf->id, &(req->transfer), &task_delegate_exec);
    }
    else
    {
        __pi_hyper_copy(conf->id, &(req->transfer), &task_delegate_exec);
    }
}

static void __pi_cl_delegate_hyper_copy(void *arg)
{
    pi_cl_hyper_req_t *req = (pi_cl_hyper_req_t *) arg;
    if (fifo == NULL)
    {
        fifo = req;
        fifo->next = NULL;
        __pi_cl_delegate_hyper_copy_exec(fifo);
    }
    else
    {
        fifo->next = req;
        fifo->next->next = NULL;
    }
}

void __pi_cl_hyper_copy_task_done(void *arg)
{
    pi_cl_hyper_req_t *req = (pi_cl_hyper_req_t *) arg;
    fifo = fifo->next;
    cl_notify_task_done(&(req->done), req->cid);
    if (fifo != NULL)
    {
        __pi_cl_delegate_hyper_copy_exec(fifo);
    }
}

void __pi_cl_hyper_copy_task_wait(void *arg)
{
    pi_cl_hyper_req_t *req = (pi_cl_hyper_req_t *) arg;
    cl_wait_task(&(req->done));
}

void __pi_cl_hyper_copy(struct pi_device *device, uint32_t hyper_addr, void *buffer,
                        uint32_t size, udma_channel_e read, pi_cl_hyper_req_t *req)
{
    struct pi_hyper_conf *conf = device->config;
    uint32_t ext_addr = hyper_addr;
    ext_addr += (conf->type == PI_HYPER_TYPE_FLASH) ? (uint32_t) REG_MBR1 : 0;
    /* Transfer struct. */
    req->transfer.hyper_addr = ext_addr;
    req->transfer.buffer = buffer;
    req->transfer.size = size;
    req->transfer.stride = 0;
    req->transfer.length = 0;
    req->transfer.channel = read;
    req->transfer.device_id = conf->id;
    req->device = device;
    req->done = 0;
    req->cid = pi_cluster_id();
    req->is_2d = 0;
    /* Callback. */
    pi_task_callback_no_mutex(&(req->cb), __pi_cl_delegate_hyper_copy, req);
    pi_cl_send_task_to_fc(&(req->cb));
}

void __pi_cl_hyper_copy_2d(struct pi_device *device, uint32_t hyper_addr,
                           void *buffer, uint32_t size, uint32_t stride,
                           uint32_t length, udma_channel_e read, pi_cl_hyper_req_t *req)
{
    struct pi_hyper_conf *conf = device->config;
    uint32_t ext_addr = hyper_addr;
    ext_addr += (conf->type == PI_HYPER_TYPE_FLASH) ? (uint32_t) REG_MBR1 : 0;
    /* Transfer struct. */
    req->transfer.hyper_addr = ext_addr;
    req->transfer.buffer = buffer;
    req->transfer.size = size;
    req->transfer.stride = stride;
    req->transfer.length = length;
    req->transfer.channel = read;
    req->transfer.device_id = conf->id;
    req->device = device;
    req->done = 0;
    req->cid = pi_cluster_id();
    req->is_2d = 1;
    /* Callback. */
    pi_task_callback_no_mutex(&(req->cb), __pi_cl_delegate_hyper_copy, req);
    pi_cl_send_task_to_fc(&(req->cb));
}

static void __pi_cl_hyper_alloc_task_done(void *arg)
{
    pi_cl_hyperram_alloc_req_t *req = (pi_cl_hyperram_alloc_req_t *) arg;
    cl_notify_task_done(&(req->done), req->cid);
}

void __pi_cl_hyper_alloc_task_wait(void *arg)
{
    pi_cl_hyperram_alloc_req_t *req = (pi_cl_hyperram_alloc_req_t *) arg;
    cl_wait_task(&(req->done));
}

static void __pi_cl_delegate_hyper_alloc(void *arg)
{
    pi_cl_hyperram_alloc_req_t *req = (pi_cl_hyperram_alloc_req_t *) arg;
    req->chunk = pi_hyperram_alloc(req->device, req->size);
    __pi_cl_hyper_alloc_task_done(arg);
}

void pi_cl_hyperram_alloc(struct pi_device *device, uint32_t size,
                          pi_cl_hyperram_alloc_req_t *req)
{
    req->device = device;
    req->chunk = 0;
    req->size = size;
    req->done = 0;
    req->cid = pi_cluster_id();
    /* Callback. */
    pi_task_callback_no_mutex(&(req->cb), __pi_cl_delegate_hyper_alloc, req);
    pi_cl_send_task_to_fc(&(req->cb));
    hal_compiler_barrier();
}

static void __pi_cl_hyper_free_task_done(void *arg)
{
    pi_cl_hyperram_free_req_t *req = (pi_cl_hyperram_free_req_t *) arg;
    cl_notify_task_done(&(req->done), req->cid);
}

void __pi_cl_hyper_free_task_wait(void *arg)
{
    pi_cl_hyperram_free_req_t *req = (pi_cl_hyperram_free_req_t *) arg;
    cl_wait_task(&(req->done));
}

static void __pi_cl_delegate_hyper_free(void *arg)
{
    pi_cl_hyperram_free_req_t *req = (pi_cl_hyperram_free_req_t *) arg;
    req->chunk = pi_hyperram_free(req->device, req->chunk, req->size);
    __pi_cl_hyper_free_task_done(arg);
}

void pi_cl_hyperram_free(struct pi_device *device, uint32_t chunk, uint32_t size,
                         pi_cl_hyperram_free_req_t *req)
{
    req->device = device;
    req->chunk = chunk;
    req->size = size;
    req->done = 0;
    req->cid = pi_cluster_id();
    /* Callback. */
    pi_task_callback_no_mutex(&(req->cb), __pi_cl_delegate_hyper_free, req);
    pi_cl_send_task_to_fc(&(req->cb));
    hal_compiler_barrier();
}

#endif  /* (FEATURE_CLUSTER == 1) */
