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

#ifndef __PI_DRIVER_DATA_H__
#define __PI_DRIVER_DATA_H__

#include "pmsis/implem/hal/hal.h"

/* This file contains data structures used throughout drivers. */

/* HYPERBUS */
/* TODO : This shall be changed. */
/*
 * pi_task:
 * data[0] = hyper_addr
 * data[1] = l2_buf
 * data[2] = size
 * data[3] = stride
 * data[4] = length
 * data[5] = channel
 * data[6] = is_emulated ?
 * data[7] = repeat_size
 */

struct hyper_driver_fifo
{
    struct pi_task *fifo_head;
    struct pi_task *fifo_tail;
    uint32_t device_id;
    uint32_t pending_emu_hyper_addr;
    uint32_t pending_emu_buffer;
    uint32_t pending_emu_size;
    uint32_t pending_emu_size_2d;
    udma_channel_e pending_emu_channel;
    uint8_t pending_emu_do_memcpy;
};

struct hyper_transfer_s
{
    uint32_t hyper_addr;
    void *buffer;
    uint32_t size;
    uint32_t stride;
    uint32_t length;
    udma_channel_e channel;
    int8_t device_id;
};

struct hyper_cb_args_s
{
    struct pi_task *cb;
    struct hyper_transfer_s transfer;
};


#endif  /* __PI_DRIVER_DATA_H__ */
