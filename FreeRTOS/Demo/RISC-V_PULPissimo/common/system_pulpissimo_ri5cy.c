/*
** ###################################################################
**     Processors:          PULPissimo_RI5CY
**                          PULPissimo_RI5CY
**
**     Compilers:           GNU C Compiler
**
**
**
**     Reference manual:    riscv-spec-v2.2
**     Version:             rev. 0.1, 2019-09-09
**     Build:
**
**     Abstract:
**         Provides a system configuration function and a global variable that
**         contains the system frequency. It configures the device and initializes
**         the oscillator (PLL) that is part of the microcontroller device.
**
**     Copyright 2016 Freescale Semiconductor, Inc.
**     Copyright 2016-2018 NXP
**     Copyright 2019 ETH Zuerich
**     All rights reserved.
**
**     SPDX-License-Identifier: BSD-3-Clause
**
**     http:                 https://pulp-platform.org
**     mail:                 balasr@iis.ee.ethz.ch
**
**     Revisions:
**     - rev. 0.1 (2019-09-09)
**         Initial version.
**
** ###################################################################
*/

/*!
 * @file system_pulpissimo_ri5cy.c
 * @version 0.1
 * @date 2019-09-09
 * @brief Device specific configuration file for PULPissimo_RI5CY (header
 *        file)
 *
 * Provides a system configuration function and a global variable that contains
 * the system frequency. It configures the device and initializes the oscillator
 * (PLL) that is part of the microcontroller device.
 */

#include "FreeRTOSConfig.h"

#include "fll.h"
#include "irq.h"
#include "soc_eu.h"

#ifndef DISABLE_WDOG
#define DISABLE_WDOG 1
#endif

/**
 * Allocate heap to special section
 */
__attribute__((section(".heap"))) uint8_t ucHeap[configTOTAL_HEAP_SIZE];

/* Inform linker script about .heap section size. Note: GNU ld seems to
 * internally represent integers with the bfd_vma type, that is a type that can
 * contain memory addresses (typdefd to some int type depending on the
 * architecture). uint32_t seems to me the most fitting candidate for rv32.
 */
uint32_t __heap_size = configTOTAL_HEAP_SIZE;

uint32_t system_core_clock = DEFAULT_SYSTEM_CLOCK;

// TODO: Fix init code
void pulp_sys_init(void)
{
	/* TODO: check this code */
	pulp_fll_init();

	/* make sure irq (itc) is a good state */
	pulp_irq_init();

	/* mtvec is set in crt0.S */

	/* deactivate all soc events as they are enabled by default */
	pulp_soc_eu_event_init();

	/* TODO: enable uart */
	/* TODO: I$ enable*/
	/* enable core level interrupt (mie) */
	irq_clint_enable();


	//    SCBC->ICACHE_ENABLE = 0xFFFFFFFF;

	/* Here we bind same Handler in M and U mode vector table, TODO, security problem */
	/* If we need to protect the access to peripheral IRQ, we need do as SysTick_Handler */
	/* by using ecall form U mode to M mode */
	//    NVIC_SetVector(FC_SOC_EVENT_IRQn, (uint32_t)__handler_wrapper_light_FC_EventHandler);

	/* Activate interrupt handler for soc event */
	//    NVIC_EnableIRQ(FC_SOC_EVENT_IRQn);

	/* Initialize malloc functions */
	//    FC_MallocInit();
	// TODO: irq enable
	//    __enable_irq();
}

//
void system_core_clock_update()
{
	//    SystemCoreClock = FLL_GetFrequency(uFLL_SOC);

	/* Need to update clock divider for each peripherals */
	//    uart_is_init     = 0;
}
