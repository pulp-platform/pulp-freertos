/*
 * Copyright (C) 2021 GreenWaves Technologies
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
 */

#ifdef CONFIG_CLUSTER

#ifndef __PI_DRIVER_CL_DMA_H__
#define __PI_DRIVER_CL_DMA_H__

#include "cluster/cl_dma.h"
#include "cluster/cl_dma_hal.h"
#include "os.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

struct pi_cl_dma_cmd_s {
	uint32_t tid;
	uint32_t cmd;
	uint32_t size;
	uint32_t stride;
	uint32_t length;
	uint32_t loc;
	uint32_t ext;
	struct pi_cl_dma_cmd_s *next;
};

/*******************************************************************************
 * Driver data
 *****************************************************************************/

extern pi_cl_dma_cmd_t *fifo_first;
extern pi_cl_dma_cmd_t *fifo_last;

/*******************************************************************************
 * Function declaration
 ******************************************************************************/

/*******************************************************************************
 * Internal functions
 ******************************************************************************/

static inline void __pi_cl_dma_copy(uint32_t ext, uint32_t loc, uint32_t len, uint32_t stride,
				    uint32_t length, uint8_t dir, uint8_t merge, uint32_t dma_cmd,
				    pi_cl_dma_cmd_t *cmd)
{
	int32_t tid = -1;
	uint32_t iter_length = (len < length) ? len : length;

	cmd->loc = loc;
	cmd->ext = ext;
	cmd->size = len;
	cmd->stride = stride;
	cmd->length = length;
	cmd->tid = tid;
	cmd->next = NULL;

	hal_compiler_barrier();
	if (fifo_first != NULL) {
		fifo_last->next = cmd;
	} else {
		cmd->loc += iter_length;
		cmd->ext += stride;
		cmd->size -= iter_length;
		hal_compiler_barrier();
		cmd->tid = hal_cl_dma_tid_get();
		hal_cl_dma_1d_transfer_push(dma_cmd, loc, ext);
		if (cmd->length != 0) {
			fifo_first = cmd;
		}
	}
	fifo_last = cmd;
	hal_compiler_barrier();
}


static inline void __pi_cl_dma_1d_copy(uint32_t ext, uint32_t loc, uint32_t len, uint8_t dir,
				       uint8_t merge, pi_cl_dma_cmd_t *cmd)
{
	uint32_t irq = disable_irq();
	uint32_t stride = 0;
	uint32_t length = 0;
	uint32_t iter_length = 0;
	uint32_t dma_cmd = 0;
	uint32_t max_len = (1 << 15);
	if (len < max_len) {
		iter_length = len;
		cmd->tid = -1;
		if (!merge) {
			cmd->tid = hal_cl_dma_tid_get();
		}
		cmd->length = 0;
		dma_cmd = hal_cl_dma_cmd_make(iter_length, dir, DMA_INC, DMA_IS_1D, DMA_ELE_ENA,
					      DMA_ILE_DIS, DMA_BLE_ENA);
		hal_cl_dma_1d_transfer_push(dma_cmd, loc, ext);
		/* cmd->cmd = hal_cl_dma_cmd_make(0, dir, DMA_INC, DMA_IS_1D, */
		/*                                DMA_ELE_ENA, DMA_ILE_DIS, DMA_BLE_ENA); */
	} else {
		iter_length = max_len;
		stride = max_len;
		length = max_len;
		dma_cmd = hal_cl_dma_cmd_make(iter_length, dir, DMA_INC, DMA_IS_1D, DMA_ELE_DIS,
					      DMA_ILE_ENA, DMA_BLE_ENA);
		cmd->cmd = hal_cl_dma_cmd_make(0, dir, DMA_INC, DMA_IS_1D, DMA_ELE_DIS, DMA_ILE_ENA,
					       DMA_BLE_ENA);
		__pi_cl_dma_copy(ext, loc, len, stride, length, dir, merge, dma_cmd, cmd);
	}
	restore_irq(irq);
}

static inline void __pi_cl_dma_2d_copy(uint32_t ext, uint32_t loc, uint32_t len, uint32_t stride,
				       uint32_t length, uint8_t dir, uint8_t merge,
				       pi_cl_dma_cmd_t *cmd)
{
	uint32_t irq = disable_irq();
	int32_t tid = -1;
	if (stride < (1 << 15)) {
		if (!merge) {
			tid = hal_cl_dma_tid_get();
			cmd->tid = tid;
		}
		cmd->length = 0;
		uint32_t dma_cmd = hal_cl_dma_cmd_make(len, dir, DMA_INC, DMA_IS_2D, DMA_ELE_ENA,
						       DMA_ILE_DIS, DMA_BLE_ENA);
		hal_compiler_barrier();
		hal_cl_dma_2d_transfer_push(dma_cmd, loc, ext, stride, length);
	} else {
		/* Emul 2D transfer, using multiple 1D transfers. */
		uint32_t iter_length = (len < length) ? len : length;
		uint32_t dma_cmd = hal_cl_dma_cmd_make(iter_length, dir, DMA_INC, DMA_IS_1D,
						       DMA_ELE_DIS, DMA_ILE_ENA, DMA_BLE_ENA);
		cmd->cmd = hal_cl_dma_cmd_make(0, dir, DMA_INC, DMA_IS_1D, DMA_ELE_DIS, DMA_ILE_ENA,
					       DMA_BLE_ENA);
		__pi_cl_dma_copy(ext, loc, len, stride, length, dir, merge, dma_cmd, cmd);
	}
	restore_irq(irq);
}

static inline void __pi_cl_dma_wait(pi_cl_dma_cmd_t *cmd)
{
	/* 1D transfer. */
	if (!cmd->length) {
		hal_cl_dma_wait(cmd->tid);
	} else {
		/* 2D transfer. */
		while (*(volatile uint32_t *)&(cmd->size) > 0) {
			hal_eu_evt_mask_wait_and_clr(1 << DMA_SW_IRQN);
		}
	}
}

/*******************************************************************************
 * Function implementation
 ******************************************************************************/

static inline void pi_cl_dma_cmd(uint32_t ext, uint32_t loc, uint32_t size, pi_cl_dma_dir_e dir,
				 pi_cl_dma_cmd_t *cmd)
{
	__pi_cl_dma_1d_copy(ext, loc, size, dir, 0, cmd);
}

static inline void pi_cl_dma_cmd_2d(uint32_t ext, uint32_t loc, uint32_t size, uint32_t stride,
				    uint32_t length, pi_cl_dma_dir_e dir, pi_cl_dma_cmd_t *cmd)
{
	__pi_cl_dma_2d_copy(ext, loc, size, stride, length, dir, 0, cmd);
}

static inline void pi_cl_dma_cmd_wait(pi_cl_dma_cmd_t *cmd)
{
	__pi_cl_dma_wait(cmd);
}

static inline void pi_cl_dma_flush()
{
	hal_cl_dma_flush();
}

/* TODO: user version. */
static inline void pi_cl_dma_memcpy(pi_cl_dma_copy_t *copy)
{
	__pi_cl_dma_1d_copy(copy->ext, copy->loc, copy->size, copy->dir, copy->merge,
			    (pi_cl_dma_cmd_t *)copy);
}

static inline void pi_cl_dma_memcpy_2d(pi_cl_dma_copy_2d_t *copy)
{
	__pi_cl_dma_2d_copy(copy->ext, copy->loc, copy->size, copy->stride, copy->length, copy->dir,
			    copy->merge, (pi_cl_dma_cmd_t *)copy);
}

static inline void pi_cl_dma_wait(void *copy)
{
	__pi_cl_dma_wait((pi_cl_dma_cmd_t *)copy);
}

#endif /* __PI_DRIVER_CL_DMA_H__ */

#endif /* CONFIG_CLUSTER */
