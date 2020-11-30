/*
 * Copyright (c) 2019, GreenWaves Technologies, Inc.
 * Copyright (c) 2020, ETH Zurich
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
#include "pmsis/drivers/uart.h"
#include "pmsis/implem/drivers/udma/uart/uart_internal.h"
#include "pmsis/implem/hal/udma/udma_uart.h"
#include "pmsis/task.h"
#include "pmsis/rtos/malloc/l2_malloc.h"
#include "pmsis/cluster/cluster_sync/cl_to_fc_delegate.h"

void __pi_cl_delegate_uart_write(struct uart_itf_data *data, void *buffer,
        uint32_t size, pi_task_t *block_callback);
void __pi_cl_delegate_uart_write_req_callback(void *arg);

// executed by event kernel, on fc
void __pi_cl_delegate_uart_write_req_callback(void *arg)
{
    struct uart_callback_args *cb_args = (struct uart_callback_args *)arg;

    __pi_uart_write(cb_args->data, cb_args->buffer, cb_args->size,
            cb_args->callback);
}

void __pi_cl_delegate_uart_write(struct uart_itf_data *data, void *buffer,
        uint32_t size, pi_task_t *block_callback)
{
    struct uart_callback_args args;

    args.data = data;
    args.buffer = buffer;
    args.size = size;
    args.callback = block_callback;
    // ---------------------
    // prepare event and push

    pi_task_t task;

    pi_task_callback_no_mutex(&task, __pi_cl_delegate_uart_write_req_callback,&args);

    pi_cl_send_task_to_fc(&task);
}

int pi_cl_uart_write(pi_device_t *device, void *buffer, uint32_t size, pi_cl_uart_req_t *req)
{
    pi_task_block_no_mutex(&req->done);
    __pi_cl_delegate_uart_write(device->data, buffer, size, &(req->done));

    hal_compiler_barrier();
    return 0;
}

int pi_cl_uart_write_byte(pi_device_t *device, uint8_t *byte, pi_cl_uart_req_t *req)
{
    pi_task_block_no_mutex(&req->done);
    __pi_cl_delegate_uart_write(device->data, byte, 1, &req->done);

    hal_compiler_barrier();
    return 0;
}

int pi_cl_uart_read(pi_device_t *device, void *addr, uint32_t size, pi_cl_uart_req_t *req);

int pi_cl_uart_read_byte(pi_device_t *device, uint8_t *byte, pi_cl_uart_req_t *req);
