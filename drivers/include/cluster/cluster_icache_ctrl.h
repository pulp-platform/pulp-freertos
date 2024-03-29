/*
 * Copyright (c) 2020, GreenWaves Technologies, Inc.
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

#ifndef __PMSIS_IMPLEM_HAL_CLUSTER_CLUSTER_ICACHE_CTRL_H__
#define __PMSIS_IMPLEM_HAL_CLUSTER_CLUSTER_ICACHE_CTRL_H__

#include <stdint.h>
#include "target.h"

/*! Cluster_icache_ctrl */
#define CL_ICACHE_CTRL_ENABLE		(0x00)
#define CL_ICACHE_CTRL_FLUSH		(0x04)
#define CL_ICACHE_CTRL_SEL_FLUSH_STATUS (0x08)
#define CL_ICACHE_CTRL_L0_FLUSH		(0x0C)

/** REGMAP_Type Register Layout Typedef */
typedef struct {
	volatile uint32_t enable; /**< Cluster instruction cache unit enable configuration register.
				   */
	volatile uint32_t flush;  /**< Cluster instruction cache unit flush command register. */
	volatile uint32_t sel_flush_status; /**< Cluster instruction cache unit selective flush
    command register. Write : flush cache for selected cores. R : pending transactions. */
	volatile uint32_t l0_flush; /**< Cluster level 0 instruction cache unit flush command
				       register. */
} cluster_icache_ctrl_t;


/* ----------------------------------------------------------------------------
   -- REGMAP Register Bitfield Access --
   ---------------------------------------------------------------------------- */

/*! @name ENABLE */
/* Cluster instruction cache enable configuration bitfield:
  - 1'b0: disabled
  - 1'b1: enabled */
#define CLUSTER_ICACHE_CTRL_ENABLE_ENABLE_MASK	(0x1)
#define CLUSTER_ICACHE_CTRL_ENABLE_ENABLE_SHIFT (0)
#define CLUSTER_ICACHE_CTRL_ENABLE_ENABLE(val)                                                     \
	(((uint32_t)(((uint32_t)(val)) << CLUSTER_ICACHE_CTRL_ENABLE_ENABLE_SHIFT)) &              \
	 CLUSTER_ICACHE_CTRL_ENABLE_ENABLE_MASK)


/*! @name FLUSH */
/* Cluster instruction cache full flush command. */
#define CLUSTER_ICACHE_CTRL_FLUSH_FLUSH_MASK  (0x1)
#define CLUSTER_ICACHE_CTRL_FLUSH_FLUSH_SHIFT (0)
#define CLUSTER_ICACHE_CTRL_FLUSH_FLUSH(val)                                                       \
	(((uint32_t)(((uint32_t)(val)) << CLUSTER_ICACHE_CTRL_FLUSH_FLUSH_SHIFT)) &                \
	 CLUSTER_ICACHE_CTRL_FLUSH_FLUSH_MASK)


/*! @name SEL_FLUSH_STATUS */
/* Cluster instruction cache selective flush address configuration bitfield. */
#define CLUSTER_ICACHE_CTRL_SEL_FLUSH_STATUS_ADDR_MASK	(0xffffffff)
#define CLUSTER_ICACHE_CTRL_SEL_FLUSH_STATUS_ADDR_SHIFT (0)
#define CLUSTER_ICACHE_CTRL_SEL_FLUSH_STATUS_ADDR(val)                                             \
	(((uint32_t)(((uint32_t)(val)) << CLUSTER_ICACHE_CTRL_SEL_FLUSH_STATUS_ADDR_SHIFT)) &      \
	 CLUSTER_ICACHE_CTRL_SEL_FLUSH_STATUS_ADDR_MASK)


/*! @name L0_FLUSH */
/* Cluster level 0 instruction cache full flush command.  */
#define CLUSTER_ICACHE_CTRL_L0_FLUSH_L0_FLUSH_MASK  (0x1)
#define CLUSTER_ICACHE_CTRL_L0_FLUSH_L0_FLUSH_SHIFT (0)
#define CLUSTER_ICACHE_CTRL_L0_FLUSH_L0_FLUSH(val)                                                 \
	(((uint32_t)(((uint32_t)(val)) << CLUSTER_ICACHE_CTRL_L0_FLUSH_L0_FLUSH_SHIFT)) &          \
	 CLUSTER_ICACHE_CTRL_L0_FLUSH_L0_FLUSH_MASK)

#define cl_glob_icache_ctrl(cid) ((cluster_icache_ctrl_t *)CL_GLOB_ICACHE_ADDR(cid))

/*! Cache enable. */
static inline void hal_cl_icache_enable(uint32_t device_id)
{
	uint32_t base = (uint32_t)cl_glob_icache_ctrl(device_id);
	uint32_t offset = CL_ICACHE_CTRL_ENABLE;
	uint32_t enable = 0xFFFFFFFF;
	hal_write32((volatile void *)(base + offset), enable);
}

static inline void hal_cl_icache_disable(uint32_t device_id)
{
	uint32_t base = (uint32_t)cl_glob_icache_ctrl(device_id);
	uint32_t offset = CL_ICACHE_CTRL_ENABLE;
	uint32_t enable = 0;
	hal_write32((volatile void *)(base + offset), enable);
}


#endif /* __PMSIS_IMPLEM_HAL_CLUSTER_CLUSTER_ICACHE_CTRL_H__ */
