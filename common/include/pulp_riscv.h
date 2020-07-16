/*
 * Copyright (C) 2020 ETH Zurich and University of Bologna
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

/* Description: RI5CY helper function
 * Authors: Robert Balas (balasr@iis.ee.ethz.ch)
 */

#ifndef __RISCV_H
#define __RISCV_H

#include <stdint.h>
#include "csr.h"

static inline uint32_t pulp_core_id()
{
	/* encoding of mhartid: {21'b0, cluster_id_i[5:0], 1'b0, core_id_i[3:0]} */
	uint32_t mhartid = csr_read(CSR_MHARTID);
	return mhartid & 0x01f;
}

static inline uint32_t pulp_cluster_id()
{
	/* encoding of mhartid {21'b0, cluster_id_i[5:0], 1'b0, core_id_i[3:0]} */
	uint32_t mhartid = csr_read(CSR_MHARTID);
	return (mhartid >> 5) & 0x3f;
}

#endif
