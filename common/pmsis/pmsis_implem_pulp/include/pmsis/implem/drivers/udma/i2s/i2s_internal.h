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

#ifndef __I2S_INTERNAL_H__
#define __I2S_INTERNAL_H__

#include "pmsis.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

struct i2s_driver_fifo_s
{
    /* Best to use only one queue since both RX & TX can be used at the same time. */
    struct pi_task *fifo_head;  /* Head of SW fifo waiting transfers. */
    struct pi_task *fifo_tail;  /* Tail of SW fifo waiting transfers. */
    uint32_t device_id;
    uint32_t frequency;
    uint32_t frame_clk_freq;
    uint32_t udma_cfg;
    uint32_t nb_open;
    uint32_t pending_size;
    size_t block_size;
    uint8_t i2s_id;
    //uint8_t shift;
    uint8_t word_size;
    uint8_t channels;
    uint8_t clk;
    uint8_t cur_buffer;
    uint8_t cur_read_buffer;
    uint8_t nb_ready_buffer;
    uint8_t reenqueue;
    pi_i2s_fmt_t format;
    pi_i2s_opt_t options;
    void *pending_buffer;
    void *pingpong_buffers[2];
    void **ring_buffer;
    uint32_t ring_buffer_nb_elem;
    uint32_t ring_buffer_head;
    uint32_t ring_buffer_tail;
    pi_mem_slab_t *mem_slab;
};

/*******************************************************************************
 * Driver data
 *****************************************************************************/

/*******************************************************************************
 * Function declaration
 ******************************************************************************/

void __pi_i2s_conf_init(struct pi_i2s_conf *conf);

int32_t __pi_i2s_open(struct pi_i2s_conf *conf);

void __pi_i2s_close(uint8_t i2s_id);

int32_t __pi_i2s_ioctl(uint8_t i2s_id, uint32_t cmd, void *arg);

int32_t __pi_i2s_read(uint8_t i2s_id, void **mem_block, size_t *size);

int32_t __pi_i2s_read_async(uint8_t i2s_id, pi_task_t *task);

int32_t __pi_i2s_read_status(pi_task_t *task, void **mem_block, size_t *size);

int32_t __pi_i2s_write(uint8_t i2s_id, void *mem_block, size_t size);

#endif  /* __I2S_INTERNAL_H__ */
