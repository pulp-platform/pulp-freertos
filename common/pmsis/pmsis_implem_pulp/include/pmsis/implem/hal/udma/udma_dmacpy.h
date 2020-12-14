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

#ifndef __PMSIS_HAL_UDMA_UDMA_DMACPY_H__
#define __PMSIS_HAL_UDMA_UDMA_DMACPY_H__

#include "pmsis/targets/target.h"
#include "pmsis/implem/hal/udma/udma_core.h"
#include "pmsis/implem/hal/udma/udma_ctrl.h"

/* DMACPY udma configuration. */
static inline void dmacpy_udma_channel_set(uint32_t device_id, uint32_t l2_buf,
                                           uint32_t size, uint32_t cfg,
                                           udma_channel_e channel)
{
    udma_enqueue_channel(&(dmacpy(device_id)->udma), l2_buf, size, cfg, channel);
}


/* DST_ADDR Register. */
static inline void dmacpy_dst_addr_set(uint32_t device_id, uint32_t addr)
{
    hal_write32(&(dmacpy(device_id)->dst_addr), addr);
}

static inline uint32_t dmacpy_dst_addr_get(uint32_t device_id)
{
    return hal_read32(&(dmacpy(device_id)->dst_addr));
}


/* SRC_ADDR Register. */
static inline void dmacpy_src_addr_set(uint32_t device_id, uint32_t addr)
{
    hal_write32(&(dmacpy(device_id)->src_addr), addr);
}

static inline uint32_t dmacpy_src_addr_get(uint32_t device_id)
{
    return hal_read32(&(dmacpy(device_id)->src_addr));
}


/* MEM_SEL Register. */
static inline void dmacpy_mem_sel_set(uint32_t device_id, uint32_t mem_sel)
{
    hal_write32(&(dmacpy(device_id)->mem_sel), mem_sel);
}

static inline uint32_t dmacpy_mem_sel_get(uint32_t device_id)
{
    return hal_read32(&(dmacpy(device_id)->mem_sel));
}


/*! DST_ADDR. */
static inline void hal_dmacpy_dst_addr_set(uint32_t device_id, uint32_t addr)
{
    dmacpy_dst_addr_set(device_id, addr);
}

static inline uint32_t hal_dmacpy_dst_addr_get(uint32_t device_id)
{
    return dmacpy_dst_addr_get(device_id);
}


/*! SRC_ADDR. */
static inline void hal_dmacpy_src_addr_set(uint32_t device_id, uint32_t addr)
{
    dmacpy_src_addr_set(device_id, addr);
}

static inline uint32_t hal_dmacpy_src_addr_get(uint32_t device_id)
{
    return dmacpy_src_addr_get(device_id);
}


/*! MEM_SEL. */
static inline void hal_dmacpy_mem_sel_set(uint32_t device_id, uint32_t mem_sel)
{
    dmacpy_mem_sel_set(device_id, mem_sel);
}

static inline uint32_t hal_dmacpy_mem_sel_get(uint32_t device_id)
{
    return dmacpy_mem_sel_get(device_id);
}


/*! UDMA. */
static inline void hal_dmacpy_enqueue(uint32_t device_id, uint32_t l2_buf,
                                      uint32_t size, uint32_t cfg, uint32_t src,
                                      uint32_t dst, uint32_t mem_sel,
                                      udma_channel_e channel)
{
    cfg |= UDMA_CORE_RX_CFG_EN(1);
    dmacpy_dst_addr_set(device_id, dst);
    dmacpy_src_addr_set(device_id, src);
    dmacpy_mem_sel_set(device_id, mem_sel);
    dmacpy_udma_channel_set(device_id, l2_buf, size, cfg, channel);
}

#endif  /* __PMSIS_HAL_UDMA_UDMA_DMACPY_H__ */
