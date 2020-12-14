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

#ifndef __PMSIS_IMPLEM_DRIVERS_UDMA_HYPERBUS_HYPERBUS_INTERNAL_H__
#define __PMSIS_IMPLEM_DRIVERS_UDMA_HYPERBUS_HYPERBUS_INTERNAL_H__

#include "pmsis.h"
#include "pmsis/implem/hal/hal.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

#define BURST_SIZE 512

/*******************************************************************************
 * Driver data
 *****************************************************************************/

/* HyperFlash status register device ready offset. */
#define DEVICE_READY_OFFSET     7
/* Write and read address */
#define SA                      0x0000

/*! @brief HyperFlash command sequence structure. */
typedef struct
{
    uint16_t data;              /*!< Command data. */
    uint16_t addr;              /*!< Commad address. */
} cmdSeq;


/*******************************************************************************
 * Function declaration
 ******************************************************************************/

/* Hyper config. */
void __pi_hyper_settings(uint8_t chip_select);

/* Pad settings. */
void __pi_hyper_pin_settings();
void __pi_hyper_pin_reset_settings();

/* Hyperflash config. */
void __pi_hyper_flash_config(struct pi_device *device);

/* Handler. */
void hyper_handler(void *arg);

/* Extern memory alloc. */
uint32_t __pi_hyper_alloc_init(malloc_t *alloc, uint32_t heapstart, int32_t size);
void __pi_hyper_alloc_deinit(malloc_t *alloc);

/* Copy in UDMA. */
void __pi_hyper_copy(int device, struct hyper_transfer_s *transfer,
                     struct pi_task *task);
void __pi_hyper_copy_2d(int device, struct hyper_transfer_s *transfer,
                        struct pi_task *task);

#endif  /* __PMSIS_IMPLEM_DRIVERS_UDMA_HYPERBUS_HYPERBUS_INTERNAL_H__ */
