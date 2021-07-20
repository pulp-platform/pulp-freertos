/*
 * Copyright (c) 2017 GreenWaves Technologies SAS. All rights reserved.
 * Copyright (c) 2020 ETH Zurich
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef __RISCV_H__
#define __RISCV_H__

#include <stdint.h>
#include <assert.h>
#include "csr.h"
#include "bits.h"
#include "io.h"

#define MSTATUS_ADDR  0x300 /*!< Machine Status Register */
#define MISA_ADDR     0x301 /*!< ISA and Extensions Register */
#define MIE_ADDR      0x304 /*!< Machine interrupt enable Register */
#define MTVEC_ADDR    0x305 /*!< Machine Trap-handler Base Address Register */
#define MSCRATCH_ADDR 0x340 /*!< Machine Scratch Register */
#define MEPC_ADDR     0x341 /*!< Machine Exception Program Counter Register */
#define MCAUSE_ADDR   0x342 /*!< Machine Trap Cause Register */
#define MTVAL_ADDR    0x343 /*!< Machine Trap Value Register */
#define MIP_ADDR      0x344 /*!< Machine Interrupt Pending Register */
#define PMPCFG0_ADDR  0x3a0 /*!< PMP Configuration Register 0 */
#define PMPADDR0_ADDR 0x3b0 /*!< PMP Address Register 0 */

#define MVENDORID_ADDR 0xF11 /*!< Vendor ID Register */
#define MARCHID_ADDR   0xF12 /*!< Architecture ID Register */
#define MIMPID_ADDR    0xF13 /*!< Implementation ID Register */
#define MHARTID_ADDR   0xF14 /*!< Hardware Thread ID Register */

#define HWLP_S0_ADDR 0x7C0 /*!< Hardware Loop Start0 Register */
#define HWLP_E0_ADDR 0x7C1 /*!< Hardware Loop End0 Register */
#define HWLP_C0_ADDR 0x7C2 /*!< Hardware Loop Count0 Register */
#define HWLP_S1_ADDR 0x7C3 /*!< Hardware Loop Start1 Register */
#define HWLP_E1_ADDR 0x7C4 /*!< Hardware Loop End1 Register */
#define HWLP_C1_ADDR 0x7C5 /*!< Hardware Loop Count1 Register */

#define UEPC_ADDR   0x041 /*!< User Exception Program Counter Register */
#define CPRIV_ADDR  0xC10 /*!< Current Privilege Level Register (unofficial) */
#define UCAUSE_ADDR 0x042 /*!< FC User Trap Cause Register */

/* MVENDORID Register Definitions */
#define MVENDORID_BANK_Pos 7U /*!< MVENDORID: BANK Position */
#define MVENDORID_BANK_Msk                                                     \
	(0x1FFFFFFUL /*<< MVENDORID_BANK_Pos*/) /*!< MVENDORID: BANK Mask */

#define MVENDORID_OFFSET_Pos 0U /*!< MVENDORID: OFFSET Position */
#define MVENDORID_OFFSET_Msk                                                   \
	(0x7FUL /*<< MVENDORID_OFFSET_Pos*/) /*!< MVENDORID: OFFSET Mask */

/* MHARTID Register Definitions */
#define MHARTID_BANK_Pos 0U /*!< MHARTID: BANK Position */
#define MHARTID_BANK_Msk                                                       \
	(0x1FUL /*<< MHARTID_BANK_Pos*/) /*!< MHARTID: BANK Mask */

/* MISA Register Definitions */
#define MISA_MXL_Pos 30U		     /*!< MISA: MXL Position */
#define MISA_MXL_Msk (0x3UL << MISA_MXL_Pos) /*!< MISA: MXL Mask */

#define MISA_EXTENSION_Pos 0U /*!< MISA: EXTENSION Position */
#define MISA_EXTENSION_Msk                                                     \
	(0x3FFFFFFUL /*<< MISA_EXTENSION_Pos*/) /*!< MISA: EXTENSION Mask */

/* MSTATUS Register Definitions */
#define MSTATUS_SD_Pos 31U		       /*!< MSTATUS: SD Position */
#define MSTATUS_SD_Msk (1UL << MSTATUS_SD_Pos) /*!< MSTATUS: SD Mask */

#define MSTATUS_TSR_Pos 22U			 /*!< MSTATUS: TSR Position */
#define MSTATUS_TSR_Msk (1UL << MSTATUS_TSR_Pos) /*!< MSTATUS: TSR Mask */

#define MSTATUS_TW_Pos 21U		       /*!< MSTATUS: TW Position */
#define MSTATUS_TW_Msk (1UL << MSTATUS_TW_Pos) /*!< MSTATUS: TW Mask */

#define MSTATUS_TVM_Pos 20U			 /*!< MSTATUS: TVM Position */
#define MSTATUS_TVM_Msk (1UL << MSTATUS_TVM_Pos) /*!< MSTATUS: TVM Mask */

#define MSTATUS_MXR_Pos 19U			 /*!< MSTATUS: MXR Position */
#define MSTATUS_MXR_Msk (1UL << MSTATUS_MXR_Pos) /*!< MSTATUS: MXR Mask */

#define MSTATUS_SUM_Pos 18U			 /*!< MSTATUS: SUM Position */
#define MSTATUS_SUM_Msk (1UL << MSTATUS_SUM_Pos) /*!< MSTATUS: SUM Mask */

#define MSTATUS_MPRV_Pos 17U /*!< MSTATUS: MPRV Position */
#define MSTATUS_MPRV_Msk (1UL << MSTATUS_MPRV_Pos) /*!< MSTATUS: MPRV Mask */

#define MSTATUS_XS_Pos 15U		       /*!< MSTATUS: XS Position */
#define MSTATUS_XS_Msk (3UL << MSTATUS_XS_Pos) /*!< MSTATUS: XS Mask */

#define MSTATUS_FS_Pos 13U		       /*!< MSTATUS: FS Position */
#define MSTATUS_FS_Msk (3UL << MSTATUS_FS_Pos) /*!< MSTATUS: FS Mask */

#define MSTATUS_MPP_Pos 11U			 /*!< MSTATUS: MPP Position */
#define MSTATUS_MPP_Msk (3UL << MSTATUS_MPP_Pos) /*!< MSTATUS: MPP Mask */

#define MSTATUS_SPP_Pos 8U			 /*!< MSTATUS: SPP Position */
#define MSTATUS_SPP_Msk (1UL << MSTATUS_SPP_Pos) /*!< MSTATUS: SPP Mask */

#define MSTATUS_MPIE_Pos 7U /*!< MSTATUS: MPIE Position */
#define MSTATUS_MPIE_Msk (1UL << MSTATUS_MPIE_Pos) /*!< MSTATUS: MPIE Mask */

#define MSTATUS_SPIE_Pos 5U /*!< MSTATUS: SPIE Position */
#define MSTATUS_SPIE_Msk (1UL << MSTATUS_SPIE_Pos) /*!< MSTATUS: SPIE Mask */

#define MSTATUS_UPIE_Pos 4U /*!< MSTATUS: UPIE Position */
#define MSTATUS_UPIE_Msk (1UL << MSTATUS_UPIE_Pos) /*!< MSTATUS: UPIE Mask */

#define MSTATUS_MIE_Pos 3U			 /*!< MSTATUS: MIE Position */
#define MSTATUS_MIE_Msk (1UL << MSTATUS_MIE_Pos) /*!< MSTATUS: MIE Mask */

#define MSTATUS_SIE_Pos 1U			 /*!< MSTATUS: SIE Position */
#define MSTATUS_SIE_Msk (1UL << MSTATUS_SIE_Pos) /*!< MSTATUS: SIE Mask */

#define MSTATUS_UIE_Pos 0U /*!< MSTATUS: UIE Position */
#define MSTATUS_UIE_Msk                                                        \
	(1UL /* << MSTATUS_UIE_Pos */) /*!< MSTATUS: UIE Mask                  \
					*/
/* MTVEC Register Definitions */
#define MTVEC_BASE_Pos 2U /*!< MTVEC: BASE Position */
#define MTVEC_BASE_Msk                                                         \
	(0x3FFFFFFFUL << MTVEC_BASE_Pos) /*!< MTVEC: BASE Mask                 \
					  */

#define MTVEC_MODE_Pos 0U /*!< MTVEC: MODE Position */
#define MTVEC_MODE_Msk (0x3UL /*<< MTVEC_MODE_Pos*/) /*!< MTVEC: MODE Mask */

/* MCAUSE Register Definitions */
#define MCAUSE_IRQ_Pos 31U			 /*!< MCAUSE: IRQ Position */
#define MCAUSE_IRQ_Msk (0x1UL << MCAUSE_IRQ_Pos) /*!< MCAUSE: IRQ Mask */

#define MCAUSE_EC_Pos 0U /*!< MCAUSE: EC Position */
#define MCAUSE_EC_Msk                                                          \
	(0x1FFFFFFFUL /*<< MCAUSE_EC_Pos*/) /*!< MCAUSE: EC Mask */


/**
  \brief   Restore the MIE bit
  \details Restore the MIE bit of MSTATUS
 */
__attribute__((always_inline)) static inline void __restore_irq(uint32_t irq)
{
	/* TODO: should probably read then write this register to not clear
	 * unintended bits */
	csr_write(MSTATUS_ADDR, irq);
}

/**
  \brief   Enable IRQ Interrupts
  \details Enables IRQ interrupts by setting the MPIE-bit in the MSTATUS.
	   Can only be executed in Privileged modes.
 */
__attribute__((always_inline)) static inline void __enable_irq(void)
{
	csr_read_set(MSTATUS_ADDR, BIT(MSTATUS_MIE_Pos));
}

/**
  \brief   Disable IRQ Interrupts
  \details Disables IRQ interrupts by clearing the MPIE-bit in the CPSR.
	   Can only be executed in Privileged modes.
 */
__attribute__((always_inline)) static inline uint32_t __disable_irq(void)
{
	uint32_t val = csr_read_clear(MSTATUS_ADDR, BIT(MSTATUS_MIE_Pos));
	return val;
}


/**
  \brief   Set mstatus Register
  \details Writes the given value to the mstatus Register.
  \param [in]    control  mstatus Register value to set
 */
__attribute__((always_inline)) static inline void
__set_mstatus(uint32_t control)
{
	csr_write(MSTATUS_ADDR, control);
}

/**
  \brief   Get MCAUSE Register
  \details Returns the content of the MCAUSE Register.
  \return               MCAUSE Register value
 */
__attribute__((always_inline)) static inline uint32_t __get_mcause(void)
{
	uint32_t result = csr_read(MCAUSE_ADDR);
	return result;
}

/**
  \brief   Get MSTATUS Register
  \details Returns the content of the MSTATUS Register.
  \return               MSTATUS Register value
 */
__attribute__((always_inline)) static inline uint32_t __get_mstatus(void)
{
	uint32_t result = csr_read(MSTATUS_ADDR);
	return result;
}


/* saner names for irqs */
#define irqn_enable	   __irq_enable
#define irqn_get_enable	   __irq_get_enable
#define irqn_disable	   __irq_disable
#define irqn_get_pending   __irq_get_pending
#define irqn_set_pending   __irq_set_pending
#define irqn_clear_pending __irq_clear_pending
#define irqn_get_active	   __irq_get_active
#define irqn_system_reset  __irq_system_reset


#endif /* __RISCV_H__ */
