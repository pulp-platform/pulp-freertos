/*
 * Copyright (c) 2019, GreenWaves Technologies, Inc.
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

#ifndef __PI_HAL_CL_DMA_H__
#define __PI_HAL_CL_DMA_H__

#include <stdint.h>
#include "cluster/event_unit.h"
#include "memory_map.h"
#include "events.h"
#include "target.h"

#define DMA_FIX	    0
#define DMA_INC	    1
#define DMA_IS_1D   0
#define DMA_IS_2D   1
#define DMA_ELE_DIS 0
#define DMA_ELE_ENA 1
#define DMA_ILE_DIS 0
#define DMA_ILE_ENA 1
#define DMA_BLE_DIS 0
#define DMA_BLE_ENA 1

typedef struct {
	volatile uint32_t CMD;	  /*!< Offset: 0x00 (R/W ) DMAMCHAN Command Base Register */
	volatile uint32_t STATUS; /*!< Offset: 0x04 (R/W)  DMAMCHAN Channle Status Register */
} DMAMCHAN_Type;

/* DMAMCHAN Registers Definitions */
#define DMAMCHAN_CMD_TID_Pos 0U					  /*!< DMAMCHAN TID Position */
#define DMAMCHAN_CMD_TID_Msk (0xFUL /* << DMAMCHAN_CMD_TID_Pos*/) /*!< DMAMCHAN TID Mask */

#define DMAMCHAN_CMD_BLE_Pos 21U /*!< DMAMCHAN Broadcast Lines Enable Position */
#define DMAMCHAN_CMD_BLE_Msk                                                                       \
	(1UL << DMAMCHAN_CMD_BLE_Pos) /*!< DMAMCHAN CMD Broadcast Lines Enable Mask */

#define DMAMCHAN_CMD_ILE_Pos 20U /*!< DMAMCHAN Intrrupt Line EnableID Position */
#define DMAMCHAN_CMD_ILE_Msk                                                                       \
	(1UL << DMAMCHAN_CMD_ILE_Pos) /*!< DMAMCHAN Intrrupt Line Enable Mask */

#define DMAMCHAN_CMD_ELE_Pos 19U /*!< DMAMCHAN CMD Event Line Enable Position */
#define DMAMCHAN_CMD_ELE_Msk                                                                       \
	(1UL << DMAMCHAN_CMD_ELE_Pos) /*!< DMAMCHAN CMD Event Line Enable Mask */

#define DMAMCHAN_CMD_2D_Pos 18U				 /*!< DMAMCHAN CMD 2D transfer Position */
#define DMAMCHAN_CMD_2D_Msk (1UL << DMAMCHAN_CMD_2D_Pos) /*!< DMAMCHAN CMD 2D transfer Mask */

#define DMAMCHAN_CMD_INC_Pos 17U			   /*!< DMAMCHAN CMD Increment Position */
#define DMAMCHAN_CMD_INC_Msk (1UL << DMAMCHAN_CMD_INC_Pos) /*!< DMAMCHAN CMD Increment Mask */

#define DMAMCHAN_CMD_TYP_Pos 16U			   /*!< DMAMCHAN CMD Type Position */
#define DMAMCHAN_CMD_TYP_Msk (1UL << DMAMCHAN_CMD_TYP_Pos) /*!< DMAMCHAN CMD Type Mask */

#define DMAMCHAN_CMD_LEN_Pos 0U				      /*!< DMAMCHAN CMD Length Position */
#define DMAMCHAN_CMD_LEN_Msk (0xFFUL /*<< DMAMCHAN_EOC_Pos*/) /*!< DMAMCHAN CMD Length Mask */

#define DMAMCHAN_CMD_2D_STRIDE_Pos 16U /*!< DMAMCHAN CMD 2D STRIDE Position */
#define DMAMCHAN_CMD_2D_STRIDE_Msk                                                                 \
	(0xFFUL << DMAMCHAN_CMD_2D_STRIDE_Pos) /*!< DMAMCHAN CMD 2D STRIDE Mask */

#define DMAMCHAN_CMD_2D_COUNT_Pos 0U /*!< DMAMCHAN CMD 2D COUNT Position */
#define DMAMCHAN_CMD_2D_COUNT_Msk                                                                  \
	(0xFFUL /* << DMAMCHAN_CMD_2D_COUNT_Pos*/) /*!< DMAMCHAN CMD 2D COUNT Mask */


#define MCHAN_BASE CL_MCHAN_ADDR
#define DMAMCHAN   ((DMAMCHAN_Type *)MCHAN_BASE)

/* Form a command from a transfer's size, 2d,... */
static inline uint32_t hal_cl_dma_cmd_make(uint16_t len, uint8_t dir, uint8_t inc, uint8_t is2d,
					   uint8_t ele, uint8_t ile, uint8_t ble)
{
	uint32_t cmd = 0;
	cmd = __BITINSERT(cmd, len, 16, DMAMCHAN_CMD_LEN_Pos);
	cmd = __BITINSERT(cmd, dir, 1, DMAMCHAN_CMD_TYP_Pos);
	cmd = __BITINSERT(cmd, inc, 1, DMAMCHAN_CMD_INC_Pos);
	cmd = __BITINSERT(cmd, is2d, 1, DMAMCHAN_CMD_2D_Pos);
	cmd = __BITINSERT(cmd, ele, 1, DMAMCHAN_CMD_ELE_Pos);
	cmd = __BITINSERT(cmd, ile, 1, DMAMCHAN_CMD_ILE_Pos);
	cmd = __BITINSERT(cmd, ble, 1, DMAMCHAN_CMD_BLE_Pos);
	return cmd;
}

/* CMD register. */
static inline void cl_dma_cmd_set(uint32_t cmd)
{
	hal_compiler_barrier();
	hal_write32(&(DMAMCHAN->CMD), cmd);
	hal_compiler_barrier();
}

static inline uint32_t cl_dma_cmd_get()
{
	return hal_read32(&(DMAMCHAN->CMD));
}

/* STATUS register. */
static inline void cl_dma_status_set(uint32_t status)
{
	hal_compiler_barrier();
	hal_write32(&(DMAMCHAN->STATUS), status);
	hal_compiler_barrier();
}

static inline uint32_t cl_dma_status_get()
{
	return hal_read32(&(DMAMCHAN->STATUS));
}


/*! CMD. */
/* Get transfer ID(TID). */
static inline uint16_t hal_cl_dma_tid_get()
{
	return cl_dma_cmd_get() & 0xFFFF;
}

/* Enqueue a 1D transfer. */
static inline void hal_cl_dma_1d_transfer_push(uint32_t cmd, uint32_t loc, uint32_t ext)
{
	cl_dma_cmd_set(cmd);
	cl_dma_cmd_set(loc);
	cl_dma_cmd_set(ext);
}

/* Enqueue a 2D transfer. */
static inline void hal_cl_dma_2d_transfer_push(uint32_t cmd, uint32_t loc, uint32_t ext,
					       uint32_t stride, uint32_t length)
{
	uint32_t conf_2d =
		(length << DMAMCHAN_CMD_2D_COUNT_Pos) | (stride << DMAMCHAN_CMD_2D_STRIDE_Pos);
	cl_dma_cmd_set(cmd);
	cl_dma_cmd_set(loc);
	cl_dma_cmd_set(ext);
	cl_dma_cmd_set(conf_2d);
}

/*! STATUS. */
/* Free a TID. */
static inline void hal_cl_dma_tid_free(uint8_t tid)
{
	cl_dma_status_set(1 << tid);
}

/* Flush TID counters : wait end of all transfers & free all counters. */
static inline void hal_cl_dma_flush()
{
	while (cl_dma_status_get() & 0xFFFF) {
		hal_eu_evt_mask_wait(1 << CL_IRQ_DMA0);
	}
	cl_dma_status_set(-1); /* -1 = 0xFFFFFFFFu */
}

/* Wait end of one transfer, then free the slot. */
/* Disabling/re-enabling IRQ is needed to synchronize transfer IDs. */
static inline void hal_cl_dma_wait(uint8_t tid)
{
	uint32_t irq = __disable_irq();
	while (cl_dma_status_get() & (1 << tid)) {
		__restore_irq(irq);
		hal_eu_evt_mask_wait(1 << CL_IRQ_DMA0);
		irq = __disable_irq();
	}
	hal_cl_dma_tid_free(tid);
	__restore_irq(irq);
}

#endif /* __PI_HAL_CL_DMA_H__ */
