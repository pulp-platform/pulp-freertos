/*
 * Copyright (C) 2019 ETH Zurich and University of Bologna
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
 * Author: Robert Balas (balasr@iis.ee.ethz.ch)
 */

/* Core-local interrupt controller for PULP. */

#include <stdint.h>
#include <assert.h>

#include "pulp_mem_map.h"
#include "properties.h"
#include "io.h"
#include "irq.h"
#include "csr.h"
#include "clic.h"

extern void (*isr_table[ISR_TABLE_SIZE])(void);

/* set interrupt handler for given interrupt id */
void irq_set_handler(int id, void (*handler)(void))
{
	assert(0 <= id && id < ISR_TABLE_SIZE);
	isr_table[id] = handler;
}

/* utility functions for PULPs external interrupt controller */
void irq_enable(int id)
{
	assert(0 <= id && id < CLIC_PARAM_NUM_SRC);
	/* enable selective hardware vectoring for interrupt. We might later
	 * make this configurable */
	writew(1 << CLIC_CLICINTATTR_SHV_BIT,
	       (uintptr_t)(PULP_CLIC_ADDR + CLIC_CLICINTATTR_REG_OFFSET(id)));

	writew(1ul, (uintptr_t)(PULP_CLIC_ADDR + CLIC_CLICINTIE_REG_OFFSET(id)));

	/* TODO: fix this quick hack which is there just to get going */
	irq_set_lvl_and_prio(id, 1, 1);
}

void irq_disable(int id)
{
	assert(0 <= id && id < CLIC_PARAM_NUM_SRC);
	writew(0 << CLIC_CLICINTATTR_SHV_BIT,
	       (uintptr_t)(PULP_CLIC_ADDR + CLIC_CLICINTATTR_REG_OFFSET(id)));
	writew(0ul, (uintptr_t)(PULP_CLIC_ADDR + CLIC_CLICINTIE_REG_OFFSET(id)));
}

void irq_pend(int id)
{
	assert(0 <= id && id < CLIC_PARAM_NUM_SRC);
	writew(1ul, (uintptr_t)(PULP_CLIC_ADDR + CLIC_CLICINTIP_REG_OFFSET(id)));
}

void irq_clear(int id)
{
	assert(0 <= id && id < CLIC_PARAM_NUM_SRC);
	writew(0ul, (uintptr_t)(PULP_CLIC_ADDR + CLIC_CLICINTIP_REG_OFFSET(id)));
}

void irq_set_lvl_and_prio(int id, int lvl, int prio)
{
	/* TODO: probe CLICINTCTLBITS */
	uint32_t nlbits = readw((uintptr_t)(PULP_CLIC_ADDR + CLIC_CLICCFG_REG_OFFSET)) >>
				  CLIC_CLICCFG_NLBITS_OFFSET &
			  CLIC_CLICCFG_NLBITS_MASK;

	uint32_t shift = 8 - nlbits;
	uint32_t val = ((((uint32_t)lvl & BIT_MASK(nlbits)) << shift |
			 ((uint32_t)prio & BIT_MASK(shift))) &
			0xff);
	writew(val, (uintptr_t)(PULP_CLIC_ADDR + CLIC_CLICINTCTL_REG_OFFSET(id)));
}

/* utility functions for the core level interrupt (CLINT) described in the
 * RISC-V privileged specification */

/* enable/disable interrupt globally (MIE bit in mstatus csr) */
uint32_t irq_clint_global_disable()
{
	uint32_t val = csr_read_clear(CSR_MSTATUS, MSTATUS_IE);
	return val;
}

uint32_t irq_clint_global_enable()
{
	uint32_t val = csr_read_set(CSR_MSTATUS, MSTATUS_IE);
	return val;
}

/* enable/disable interrupts selectively (in mie csr) */
uint32_t irq_clint_disable(int id)
{
	/* these appear hardwired to zero in clic mode */
	uint32_t val = csr_read_clear(CSR_MIE, 1ul << id);
	return val;
}

uint32_t irq_clint_enable(int id)
{
	/* these appear hardwired to zero in clic mode */
	uint32_t val = csr_read_set(CSR_MIE, 1ul << id);
	return val;
}


void pulp_irq_init()
{
	/* min threshold, thereby propagating all interrupts */
	csr_write(CSR_MINTTHRESH, 0x0);
	/* set nlbits to four which gives 4 bits for level and priority */
	/* TODO: implement freertos level interrupts */
	writeb((0x4 << CLIC_CLICCFG_NLBITS_OFFSET), PULP_CLIC_ADDR + CLIC_CLICCFG_REG_OFFSET);
}
