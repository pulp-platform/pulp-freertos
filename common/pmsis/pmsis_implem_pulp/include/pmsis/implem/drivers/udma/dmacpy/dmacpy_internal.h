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


#ifndef __PMSIS_DRIVER_UDMA_DMACPY_DMACPY_INTERNAL_H__
#define __PMSIS_DRIVER_UDMA_DMACPY_DMACPY_INTERNAL_H__

#include "pmsis.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

#if defined(TRACE_DMACPY)
#define DMACPY_TRACE(...) PI_LOG_DBG(__func__, __VA_ARGS__)
#define DMACPY_TRACE_ERR(...) PI_LOG_ERR(__func__, __VA_ARGS__)
#else
#define DMACPY_TRACE(...) ((void) 0)
#define DMACPY_TRACE_ERR(...) ((void) 0)
#endif  /* TRACE_DMACPY */

struct dmacpy_driver_fifo_s
{
    /* Best to use only one queue since both RX & TX can be used at the same time. */
    struct pi_task *fifo_head;  /*!< Head of SW fifo waiting transfers. */
    struct pi_task *fifo_tail;  /*!< Tail of SW fifo waiting transfers. */
    uint32_t device_id;         /*!< Device ID. */
    uint32_t nb_open;           /*!< Number of users on this device. */
};

/*
 * pi_task:
 * data[0] = src
 * data[1] = dst
 * data[2] = size
 * data[3] = dir
 * data[4] = repeat_size
 */

/*******************************************************************************
 * Driver data
 *****************************************************************************/

/*******************************************************************************
 * Function declaration
 ******************************************************************************/

void __pi_dmacpy_conf_init(struct pi_dmacpy_conf *conf);

int32_t __pi_dmacpy_open(struct pi_dmacpy_conf *conf,
                         struct dmacpy_driver_fifo_s **device_data);

void __pi_dmacpy_close(uint32_t device_id);

int32_t __pi_dmacpy_copy(uint32_t device_id, void *src, void *dst,
                         uint32_t size, pi_dmacpy_dir_e dir, struct pi_task *task);

#endif  /* __PMSIS_DRIVER_UDMA_DMACPY_DMACPY_INTERNAL_H__ */
