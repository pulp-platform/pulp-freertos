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
 */

/* Driver to configure PULP timer as periodic interrupt source */
/* Author: Robert Balas (balasr@iis.ee.ethz.ch)
 *         Germain Haugou (germain.haugou@iis.ee.ethz.ch)
 */

#include <stdint.h>
#include <stdbool.h>

#include "memory_map.h"
#include "io.h"
#include "bits.h"

#include "timer.h"
#include "timer_hal.h"
#include "timer_irq.h"

int timer_irq_init_fc(uint32_t ticks, unsigned int timer_cfg, timer_id_t timer_id)
{
	/* set the interrupt interval (tick rate) */
	set_timer_irq_freq(ticks, timer_id);

	/* We use only one of the 32-bit timer (LO, HI), leaving the other half available
	 * as an additional timer. We didn't opt for using both together as
	 * 64-bit timer.

	 * `timer_id` indicates the 32-bit timer in use
	 * `timer_cfg` indicates the timer configuration of said timer

	 * Enable timer (TIMER_CFG_XX_ENABLE_MASK), use 32khz ref clock as
	 * source (TIMER_CFG_XX_CLKCFG_MASK). Timer will reset automatically
	 * (TIMER_CFG_XX_MODE_MASK) to zero after causing an interrupt
	 * (TIMER_CFG_XX_IRQEN_MASK). Also reset timer to start from a clean
	 * state (TIMER_CFG_XX_RESET_MASK).
	 */

	set_timer_cfg(timer_cfg, timer_id);

	return 0;
}

/* TODO: implement */
uint32_t timer_irq_clock_elapsed_fc()
{
	return 0;
}

uint32_t timer_irq_cycle_get_32_fc()
{
	return readw((uintptr_t)(FC_TIMER_ADDR + TIMER_CNT_LO_OFFSET));
}
