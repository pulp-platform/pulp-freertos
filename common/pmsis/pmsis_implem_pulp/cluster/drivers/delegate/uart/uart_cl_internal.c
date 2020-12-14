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

#if defined(FEATURE_CLUSTER)

#include "pmsis.h"
#include "pmsis/drivers/uart.h"
#include "pmsis/implem/drivers/udma/uart/uart_internal.h"
#include "pmsis/implem/cluster/delegate/uart/uart_cl_internal.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Driver data
 *****************************************************************************/

/*******************************************************************************
 * Function declaration
 ******************************************************************************/

/**
 * \brief Cluster uart delegate copy.
 *
 * This function is executed by FC.
 */
static void __pi_cl_uart_delegate_copy(void *arg);

/**
 * \brief Cluster uart copy.
 *
 * The cluster core prepares a delegate transfer and send it to FC.
 * This function is executed by cluster cores.
 */
static inline int32_t __pi_cl_uart_copy(pi_device_t *device, void *buffer,
                                        uint32_t size, udma_channel_e channel,
                                        pi_cl_uart_req_t *req);

/*******************************************************************************
 * Internal functions
 ******************************************************************************/

static void __pi_cl_uart_delegate_copy(void *arg)
{
    pi_cl_uart_req_t *req = (pi_cl_uart_req_t *) arg;
    __pi_uart_copy((struct uart_itf_data_s *) req->task_done.data[4],
                   req->task_done.data[0], req->task_done.data[1],
                   req->task_done.data[2], &(req->task_done));
}

static inline int32_t __pi_cl_uart_copy(pi_device_t *device, void *buffer,
                                        uint32_t size, udma_channel_e channel,
                                        pi_cl_uart_req_t *req)
{
    struct uart_itf_data_s *data = (struct uart_itf_data_s *) device->data;
    uint32_t l2_buf = (uint32_t) buffer;
    req->task_done.data[0] = l2_buf;
    req->task_done.data[1] = size;
    req->task_done.data[2] = channel;
    req->task_done.data[4] = (uint32_t) data;
    /* FC Callback. */
    pi_task_callback_no_mutex(&(req->req_cb), __pi_cl_uart_delegate_copy, req);
    /* Send delegate task to FC. */
    pi_cl_send_task_to_fc(&(req->req_cb));
    return 0;
}

/*******************************************************************************
 * Function implementation
 ******************************************************************************/

int pi_cl_uart_write(pi_device_t *device, void *buffer, uint32_t size, pi_cl_uart_req_t *req)
{
    pi_task_block(&(req->task_done));
    __pi_cl_uart_copy(device, buffer, size, TX_CHANNEL, req);
    return 0;
}

int pi_cl_uart_write_byte(pi_device_t *device, uint8_t *byte, pi_cl_uart_req_t *req)
{
    pi_task_block_no_mutex(&(req->task_done));
    __pi_cl_uart_copy(device, byte, 1, TX_CHANNEL, req);
    return 0;
}

int pi_cl_uart_read(pi_device_t *device, void *buffer, uint32_t size, pi_cl_uart_req_t *req)
{
    pi_task_block_no_mutex(&(req->task_done));
    __pi_cl_uart_copy(device, buffer, size, RX_CHANNEL, req);
    return 0;
}

int pi_cl_uart_read_byte(pi_device_t *device, uint8_t *byte, pi_cl_uart_req_t *req)
{
    pi_task_block_no_mutex(&(req->task_done));
    __pi_cl_uart_copy(device, byte, 1, RX_CHANNEL, req);
    return 0;
}

#endif  /* FEATURE_CLUSTER */
