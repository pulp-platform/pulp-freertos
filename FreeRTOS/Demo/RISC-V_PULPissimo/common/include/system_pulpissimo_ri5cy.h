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
 * @file system_pulpissimo_ri5cy.h
 * @version 0.1
 * @date 2019-09-09
 * @brief Device specific configuration file for PULPissimo_RI5CY (header
 *        file)
 *
 * Provides a system configuration function and a global variable that contains
 * the system frequency. It configures the device and initializes the oscillator
 * (PLL) that is part of the microcontroller device.
 */

#ifndef _SYSTEM_PULPISSIMO_RI5CY_H_
#define _SYSTEM_PULPISSIMO_RI5CY_H_          /**< Symbol preventing repeated inclusion */

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

#include "FreeRTOSConfig.h"
/* Define clock source values */
// TODO: check
//#define CPU_XTAL_CLK_HZ                 8000000u            /* Value of the external crystal or oscillator clock frequency in Hz */

/* Low power mode enable */
/* SMC_PMPROT: AHSRUN=1, AVLP=1,ALLS=1,AVLLS=0x3 */
// TODO: check
// #define SYSTEM_SMC_PMPROT_VALUE        0xABu               /* SMC_PMPROT */
// #define SYSTEM_SMC_PMCTRL_VALUE        0x0u                /* SMC_PMCTRL */


/**
 * @brief System clock frequency (core clock)
 *
 * The system clock frequency supplied to the SysTick timer and the processor
 * core clock. This variable can be used by the user application to setup the
 * SysTick timer or configure other parameters. It may also be used by debugger to
 * query the frequency of the debug timer or configure the trace clock speed
 * SystemCoreClock is initialized with a correct predefined value.
 */
extern uint32_t SystemCoreClock;

/**
 * @brief Setup the microcontroller system.
 *
 * Typically this function configures the oscillator (PLL) that is part of the
 * microcontroller device. For systems with variable clock speed it also updates
 * the variable SystemCoreClock. SystemInit is called from startup_device file.
 */
void SystemInit (void);

/**
 * @brief Updates the SystemCoreClock variable.
 *
 * It must be called whenever the core clock is changed during program
 * execution. SystemCoreClockUpdate() evaluates the clock register settings and calculates
 * the current core clock.
 */
void SystemCoreClockUpdate (void);

/**
 * @brief SystemInit function hook.
 *
 * This weak function allows to call specific initialization code during the
 * SystemInit() execution.This can be used when an application specific code needs
 * to be called as close to the reset entry as possible (for example the Multicore
 * Manager MCMGR_EarlyInit() function call).
 * NOTE: No global r/w variables can be used in this hook function because the
 * initialization of these variables happens after this function.
 */
void SystemInitHook (void);

/**
 * @brief System IRQ handler which dispatches specific IRQ to corresponding registered handler.
 *
 * It is called from IRQ exception context and dispatches to registered handler according to
 * MCAUSE interrupt number.
 *
 * @param mcause IRQ acknowledge value read from MCAUSE
 */
void SystemIrqHandler(uint32_t mcause);

/**
 * @brief Get IRQ nesting level of current context.
 *
 * If the return value is 0, then the context is not ISR, otherwise the context is ISR.
 *
 * @return IRQ nesting level
 */
uint32_t SystemGetIRQNestingLevel (void);

/**
 * @brief Setup systick for RTOS system.
 *
 * @param tickRateHz Tick number per second
 * @param intPriority IRQ interrupt priority (the smaller, the higher priority)
 */
void SystemSetupSystick (uint32_t tickRateHz, uint32_t intPriority);

/**
 * @brief Clear systick flag status so that next tick interrupt may occur.
 */
void SystemClearSystickFlag (void);

#define SysTick_Handler LPIT1_IRQHandler

/**
 * @brief Sysem is in ISR or not.
 */
bool SystemInISR(void);

// TODO: event unit stuff
///**
// * @brief Set interrupt priority in Event unit.
// */
//void EVENT_SetIRQPriority(IRQn_Type IRQn, uint8_t intPriority);

/* Priority setting macro remap. */
#define NVIC_SetPriority EVENT_SetIRQPriority

/**
 * @brief Reset the system.
 */
void EVENT_SystemReset(void);

#define NVIC_SystemReset EVENT_SystemReset

#ifdef __cplusplus
}
#endif

#endif  /* _SYSTEM_PULPISSIMO_RI5CY_H_ */
