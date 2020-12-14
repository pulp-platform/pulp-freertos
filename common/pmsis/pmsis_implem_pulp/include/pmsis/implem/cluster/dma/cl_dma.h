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

#if (FEATURE_CLUSTER == 1)

#ifndef __PI_DRIVER_CL_DMA_H__
#define __PI_DRIVER_CL_DMA_H__

#include "pmsis.h"
#include "pmsis/implem/hal/hal.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

struct pi_cl_dma_cmd_s
{
    uint32_t tid;
    uint32_t cmd;
    uint16_t size;
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

static inline void __pi_cl_dma_1d_copy(uint32_t ext, uint32_t loc, uint16_t len,
                                       uint8_t dir, uint8_t merge, pi_cl_dma_cmd_t *cmd)
{
    int32_t tid = -1;
    if (!merge)
    {
        tid = hal_cl_dma_tid_get();
    }
    cmd->tid = tid;
    cmd->length = 0;            /* Used to check if 2D transfer. */
    uint32_t dma_cmd = hal_cl_dma_cmd_make(len, dir, DMA_INC, DMA_IS_1D,
                                           DMA_ELE_ENA, DMA_ILE_DIS, DMA_BLE_ENA);
    hal_compiler_barrier();
    hal_cl_dma_1d_transfer_push(dma_cmd, loc, ext);
}

static inline void __pi_cl_dma_2d_copy(uint32_t ext, uint32_t loc, uint32_t len,
                                       uint32_t stride, uint32_t length,
                                       uint8_t dir, uint8_t merge, pi_cl_dma_cmd_t *cmd)
{
    int32_t tid = -1;
    if (!merge)
    {
        tid = hal_cl_dma_tid_get();
    }
    if (stride < (1 << 15))
    {
        cmd->tid = tid;
        cmd->length = 0;
        uint32_t dma_cmd = hal_cl_dma_cmd_make(len, dir, DMA_INC, DMA_IS_2D,
                                               DMA_ELE_ENA, DMA_ILE_DIS, DMA_BLE_ENA);
        hal_compiler_barrier();
        hal_cl_dma_2d_transfer_push(dma_cmd, loc, ext, stride, length);
    }
    else
    {
        /* Emul 2D transfer, using multiple 1D transfers. */
        uint32_t iter_length = (len < length) ? len : length;
        uint32_t dma_cmd = hal_cl_dma_cmd_make(iter_length, dir, DMA_INC, DMA_IS_1D,
                                               DMA_ELE_DIS, DMA_ILE_ENA, DMA_BLE_ENA);
        cmd->loc = loc + iter_length;
        cmd->ext = ext + stride;
        cmd->size = len - iter_length;
        cmd->stride = stride;
        cmd->length = length;
        cmd->cmd = hal_cl_dma_cmd_make(0, dir, DMA_INC, DMA_IS_1D,
                                       DMA_ELE_DIS, DMA_ILE_ENA, DMA_BLE_ENA);
        cmd->tid = tid;
        cmd->next = NULL;

        hal_compiler_barrier();
        uint32_t irq = disable_irq();
        if (fifo_first != NULL)
        {
            fifo_last->next = cmd;
        }
        else
        {
            hal_compiler_barrier();
            hal_cl_dma_1d_transfer_push(dma_cmd, loc, ext);
            fifo_first = cmd;
        }
        fifo_last = cmd;
        hal_compiler_barrier();
        restore_irq(irq);
    }
}

static inline void __pi_cl_dma_wait(pi_cl_dma_cmd_t *cmd)
{
    /* 1D transfer. */
    if (!cmd->length)
    {
        hal_cl_dma_wait(cmd->tid);
    }
    else
    {
        while (*(volatile uint16_t *) &(cmd->size) > 0)
        {
            hal_eu_evt_mask_wait(1 << DMA_SW_IRQN);
        }
        hal_cl_dma_tid_free(cmd->tid);
    }
}

/*******************************************************************************
 * Function implementation
 ******************************************************************************/

static inline void pi_cl_dma_cmd(uint32_t ext, uint32_t loc, uint32_t size,
                                 pi_cl_dma_dir_e dir, pi_cl_dma_cmd_t *cmd)
{
    __pi_cl_dma_1d_copy(ext, loc, size, dir, 0, cmd);
}

static inline void pi_cl_dma_cmd_2d(uint32_t ext, uint32_t loc, uint32_t size,
                                    uint32_t stride, uint32_t length,
                                    pi_cl_dma_dir_e dir, pi_cl_dma_cmd_t *cmd)
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
    __pi_cl_dma_1d_copy(copy->ext, copy->loc, copy->size,
                        copy->dir, copy->merge, (pi_cl_dma_cmd_t *) copy);
}

static inline void pi_cl_dma_memcpy_2d(pi_cl_dma_copy_2d_t *copy)
{
    __pi_cl_dma_2d_copy(copy->ext, copy->loc, copy->size,
                        copy->stride, copy->length,
                        copy->dir, copy->merge, (pi_cl_dma_cmd_t *) copy);
}

static inline void pi_cl_dma_wait(void *copy)
{
    __pi_cl_dma_wait((pi_cl_dma_cmd_t *) copy);
}

#endif  /* __PI_DRIVER_CL_DMA_H__ */

#endif  /* FEATURE_CLUSTER */
