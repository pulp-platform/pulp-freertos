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

#include "pmsis/implem/cluster/dma/cl_dma.h"

void pi_cl_dma_2d_handler()
{
    pi_cl_dma_cmd_t *copy = fifo_first;
    hal_compiler_barrier();
    if (!copy->size)
    {
        copy = copy->next;
        hal_eu_cluster_evt_trig_set(DMA_SW_IRQN, 0);
    }
    hal_compiler_barrier();

    hal_compiler_barrier();
    if ((copy != NULL) && (copy->size))
    {
        uint32_t iter_length = (copy->size < copy->length) ? copy->size : copy->length;
        uint32_t dma_cmd = copy->cmd | (iter_length << DMAMCHAN_CMD_LEN_Pos);
        uint32_t loc = copy->loc;
        uint32_t ext = copy->ext;
        copy->loc = loc + iter_length;
        copy->ext = ext + copy->stride;
        copy->size = copy->size - iter_length;
        hal_cl_dma_1d_transfer_push(dma_cmd, loc, ext);
    }
    hal_compiler_barrier();
}

#endif  /* FEATURE_CLUSTER */

