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

#include <stdint.h>
#include <stddef.h>
#include "fc_event.h"
#include "memory_map.h"
#include "pulp_mem_map.h"
#include "riscv.h"
#ifdef CONFIG_CLIC
#include "clic.h"
#endif


static void fc_event_null_event(void *arg);

static volatile pi_fc_event_handler_t fc_event_handlers[NB_SOC_EVENTS];

static void fc_event_null_event(void *arg)
{
	return;
}

void pi_fc_event_handler_init(uint32_t fc_event_irq)
{
	/* TODO: fix this mess, that should be 8 32-bit writes */
	/* open the mask for fc_soc_event irq */
	for (int i = 0; i < NB_SOC_EVENTS; i++) {
		pi_fc_event_handler_clear((uint32_t)i);
	}
	irq_enable((int)fc_event_irq);
#ifdef CONFIG_CLIC
	irq_set_trigger_type((int)fc_event_irq, CLIC_TRIG_LEVEL | CLIC_TRIG_POSITIVE);
#endif
}

void pi_fc_event_handler_set(uint32_t event_id,
			     pi_fc_event_handler_t event_handler)
{
	fc_event_handlers[event_id] = event_handler;
}

void pi_fc_event_handler_clear(uint32_t event_id)
{
	fc_event_handlers[event_id] =
		(pi_fc_event_handler_t)fc_event_null_event;
}

/* TODO: fix */
__attribute__((section(".text"))) void fc_soc_event_handler(void)
{
	uint32_t event = 0;
	/* When we are using the clic, we don't have the SIMPLE_IRQ FIFO
	 * register anymore. Instead, we have a minimal event_to_level_int
	 * convertor that takes over the role of generating a level sensitive
	 * interrupt on the CLIC and contains a small register storing the event
	 * data. This data needs to be read to clear the interrupt */
#ifdef CONFIG_CLIC
	event = readw((uintptr_t)PULP_EVENT_TO_INT_ADDR);
#else
	/* Pop one event element from the FIFO */
	event = SIMPLE_IRQ->FIFO;
#endif
	event &= 0xff;

	/* redirect to handler with jump table */
	if (fc_event_handlers[event] != NULL) {
		fc_event_handlers[event]((void *)event);
	}
}
