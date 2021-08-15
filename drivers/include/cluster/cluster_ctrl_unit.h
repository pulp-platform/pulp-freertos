/*
 * Copyright (c) 2020, GreenWaves Technologies, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of GreenWaves Technologies, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __PMSIS_IMPLEM_HAL_CLUSTER_CLUSTER_CTRL_UNIT_H__
#define __PMSIS_IMPLEM_HAL_CLUSTER_CLUSTER_CTRL_UNIT_H__

#include <stdint.h>
#include "target.h"
#include "memory_map.h"

/*! Cluster_ctrl_unit */
#define CL_CTRL_UNIT_EOC	 (0x00)
#define CL_CTRL_UNIT_FETCH_EN	 (0x08)
#define CL_CTRL_UNIT_CLOCK_GATE	 (0x20)
#define CL_CTRL_UNIT_BOOT_ADDR_0 (0x40)

/** REGMAP_Type Register Layout Typedef */
typedef struct {
	volatile uint32_t eoc;		 /**< End Of Computation status register. */
	volatile uint32_t reserved_0[1]; /**< Reserved/Unused. */
	volatile uint32_t fetch_en;	 /**< Cluster cores fetch enable configuration register. */
	volatile uint32_t reserved_1[5]; /**< Reserved/Unused. */
	volatile uint32_t clock_gate;	 /**< Cluster clock gate configuration register. */
	volatile uint32_t reserved_2[1]; /**< Reserved/Unused. */
	volatile uint32_t dbg_halt_status_resume; /**< Cluster cores debug halt status/resume
						     register. */
	volatile uint32_t reserved_3[3];	  /**< Reserved/Unused. */
	volatile uint32_t dbg_halt_mask; /**< Cluster cores debug halt mask configuration register.
					  */
	volatile uint32_t reserved_4[1]; /**< Reserved/Unused. */
	volatile uint32_t boot_addr0;	 /**< Cluster core 0 boot address configuration register. */
	volatile uint32_t boot_addr1;	 /**< Cluster core 1 boot address configuration register. */
	volatile uint32_t boot_addr2;	 /**< Cluster core 2 boot address configuration register. */
	volatile uint32_t boot_addr3;	 /**< Cluster core 3 boot address configuration register. */
	volatile uint32_t boot_addr4;	 /**< Cluster core 4 boot address configuration register. */
	volatile uint32_t boot_addr5;	 /**< Cluster core 5 boot address configuration register. */
	volatile uint32_t boot_addr6;	 /**< Cluster core 6 boot address configuration register. */
	volatile uint32_t boot_addr7;	 /**< Cluster core 7 boot address configuration register. */
	volatile uint32_t reserved_5[8]; /**< Reserved/Unused. */
	volatile uint32_t tcdm_arb_policy_ch0; /**< TCDM arbitration policy ch0 for cluster cores
						  configuration register. */
	volatile uint32_t reserved_6[1];       /**< Reserved/Unused. */
	volatile uint32_t tcdm_arb_policy_ch1; /**< TCDM arbitration policy ch1 for DMA/HWCE
						  configuration register. */
	volatile uint32_t reserved_7[13];      /**< Reserved/Unused. */
	volatile uint32_t tcdm_arb_policy_ch0_rep; /**< Read only duplicate of TCDM_ARB_POLICY_CH0
						      register */
	volatile uint32_t reserved_8[1];	   /**< Reserved/Unused. */
	volatile uint32_t tcdm_arb_policy_ch1_rep; /**< Read only duplicate of TCDM_ARB_POLICY_CH1
						      register */
} cluster_ctrl_unit_t;

#define cl_glob_ctrl_unit(cid) ((cluster_ctrl_unit_t *)CL_CTRL_GLOB_ADDR(cid))

/*! @name EOC */
/* End of computation status flag bitfield:
  - 1'b0: program execution under going
  - 1'b1: end of computation reached */
#define CLUSTER_CTRL_UNIT_EOC_EOC_MASK	(0x1)
#define CLUSTER_CTRL_UNIT_EOC_EOC_SHIFT (0)
#define CLUSTER_CTRL_UNIT_EOC_EOC(val)                                                             \
	(((uint32_t)(((uint32_t)(val)) << CLUSTER_CTRL_UNIT_EOC_EOC_SHIFT)) &                      \
	 CLUSTER_CTRL_UNIT_EOC_EOC_MASK)


/*! @name FETCH_EN */
/* Core 0 fetch enable configuration bitfield:
  - 1'b0: disabled
  - 1'b1: enabled */
#define CLUSTER_CTRL_UNIT_FETCH_EN_CORE0_MASK  (0x1)
#define CLUSTER_CTRL_UNIT_FETCH_EN_CORE0_SHIFT (0)
#define CLUSTER_CTRL_UNIT_FETCH_EN_CORE0(val)                                                      \
	(((uint32_t)(((uint32_t)(val)) << CLUSTER_CTRL_UNIT_FETCH_EN_CORE0_SHIFT)) &               \
	 CLUSTER_CTRL_UNIT_FETCH_EN_CORE0_MASK)

/* Core 1 fetch enable configuration bitfield:
  - 1'b0: disabled
  - 1'b1: enabled */
#define CLUSTER_CTRL_UNIT_FETCH_EN_CORE1_MASK  (0x2)
#define CLUSTER_CTRL_UNIT_FETCH_EN_CORE1_SHIFT (1)
#define CLUSTER_CTRL_UNIT_FETCH_EN_CORE1(val)                                                      \
	(((uint32_t)(((uint32_t)(val)) << CLUSTER_CTRL_UNIT_FETCH_EN_CORE1_SHIFT)) &               \
	 CLUSTER_CTRL_UNIT_FETCH_EN_CORE1_MASK)

/* Core 2 fetch enable configuration bitfield:
  - 1'b0: disabled
  - 1'b1: enabled */
#define CLUSTER_CTRL_UNIT_FETCH_EN_CORE2_MASK  (0x4)
#define CLUSTER_CTRL_UNIT_FETCH_EN_CORE2_SHIFT (2)
#define CLUSTER_CTRL_UNIT_FETCH_EN_CORE2(val)                                                      \
	(((uint32_t)(((uint32_t)(val)) << CLUSTER_CTRL_UNIT_FETCH_EN_CORE2_SHIFT)) &               \
	 CLUSTER_CTRL_UNIT_FETCH_EN_CORE2_MASK)

/* Core 3 fetch enable configuration bitfield:
  - 1'b0: disabled
  - 1'b1: enabled */
#define CLUSTER_CTRL_UNIT_FETCH_EN_CORE3_MASK  (0x8)
#define CLUSTER_CTRL_UNIT_FETCH_EN_CORE3_SHIFT (3)
#define CLUSTER_CTRL_UNIT_FETCH_EN_CORE3(val)                                                      \
	(((uint32_t)(((uint32_t)(val)) << CLUSTER_CTRL_UNIT_FETCH_EN_CORE3_SHIFT)) &               \
	 CLUSTER_CTRL_UNIT_FETCH_EN_CORE3_MASK)

/* Core 4 fetch enable configuration bitfield:
  - 1'b0: disabled
  - 1'b1: enabled */
#define CLUSTER_CTRL_UNIT_FETCH_EN_CORE4_MASK  (0x10)
#define CLUSTER_CTRL_UNIT_FETCH_EN_CORE4_SHIFT (4)
#define CLUSTER_CTRL_UNIT_FETCH_EN_CORE4(val)                                                      \
	(((uint32_t)(((uint32_t)(val)) << CLUSTER_CTRL_UNIT_FETCH_EN_CORE4_SHIFT)) &               \
	 CLUSTER_CTRL_UNIT_FETCH_EN_CORE4_MASK)

/* Core 5 fetch enable configuration bitfield:
  - 1'b0: disabled
  - 1'b1: enabled */
#define CLUSTER_CTRL_UNIT_FETCH_EN_CORE5_MASK  (0x20)
#define CLUSTER_CTRL_UNIT_FETCH_EN_CORE5_SHIFT (5)
#define CLUSTER_CTRL_UNIT_FETCH_EN_CORE5(val)                                                      \
	(((uint32_t)(((uint32_t)(val)) << CLUSTER_CTRL_UNIT_FETCH_EN_CORE5_SHIFT)) &               \
	 CLUSTER_CTRL_UNIT_FETCH_EN_CORE5_MASK)

/* Core 6 fetch enable configuration bitfield:
  - 1'b0: disabled
  - 1'b1: enabled */
#define CLUSTER_CTRL_UNIT_FETCH_EN_CORE6_MASK  (0x40)
#define CLUSTER_CTRL_UNIT_FETCH_EN_CORE6_SHIFT (6)
#define CLUSTER_CTRL_UNIT_FETCH_EN_CORE6(val)                                                      \
	(((uint32_t)(((uint32_t)(val)) << CLUSTER_CTRL_UNIT_FETCH_EN_CORE6_SHIFT)) &               \
	 CLUSTER_CTRL_UNIT_FETCH_EN_CORE6_MASK)

/* Core 7 fetch enable configuration bitfield:
  - 1'b0: disabled
  - 1'b1: enabled */
#define CLUSTER_CTRL_UNIT_FETCH_EN_CORE7_MASK  (0x80)
#define CLUSTER_CTRL_UNIT_FETCH_EN_CORE7_SHIFT (7)
#define CLUSTER_CTRL_UNIT_FETCH_EN_CORE7(val)                                                      \
	(((uint32_t)(((uint32_t)(val)) << CLUSTER_CTRL_UNIT_FETCH_EN_CORE7_SHIFT)) &               \
	 CLUSTER_CTRL_UNIT_FETCH_EN_CORE7_MASK)


/*! @name CLOCK_GATE */
/* Cluster clock gate configuration bitfield:
  - 1'b0: disabled
  - 1'b1: enabled */
#define CLUSTER_CTRL_UNIT_CLOCK_GATE_EN_MASK  (0x1)
#define CLUSTER_CTRL_UNIT_CLOCK_GATE_EN_SHIFT (0)
#define CLUSTER_CTRL_UNIT_CLOCK_GATE_EN(val)                                                       \
	(((uint32_t)(((uint32_t)(val)) << CLUSTER_CTRL_UNIT_CLOCK_GATE_EN_SHIFT)) &                \
	 CLUSTER_CTRL_UNIT_CLOCK_GATE_EN_MASK)


/*! @name DBG_HALT_STATUS_RESUME */
/* W : Core 0 debug halt status flag bitfield:
  - 1'b0: running
  - 1'b1: halted
  R : Core 0 debug resume configuration bitfield:
  - 1'b0: stay halted
  - 1'b1: resume core 0 */
#define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_RESUME_CORE0_MASK  (0x1)
#define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_RESUME_CORE0_SHIFT (0)
#define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_RESUME_CORE0(val)                                        \
	(((uint32_t)(((uint32_t)(val)) << CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_RESUME_CORE0_SHIFT)) & \
	 CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_RESUME_CORE0_MASK)

/* W : Core 1 debug halt status flag bitfield:
  - 1'b0: running
  - 1'b1: halted
  R : Core 1 debug resume configuration bitfield:
  - 1'b0: stay halted
  - 1'b1: resume core 1 */
#define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_RESUME_CORE1_MASK  (0x2)
#define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_RESUME_CORE1_SHIFT (1)
#define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_RESUME_CORE1(val)                                        \
	(((uint32_t)(((uint32_t)(val)) << CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_RESUME_CORE1_SHIFT)) & \
	 CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_RESUME_CORE1_MASK)

/* W : Core 2 debug halt status flag bitfield:
  - 1'b0: running
  - 1'b1: halted
  R : Core 2 debug resume configuration bitfield:
  - 1'b0: stay halted
  - 1'b1: resume core 2 */
#define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_RESUME_CORE2_MASK  (0x4)
#define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_RESUME_CORE2_SHIFT (2)
#define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_RESUME_CORE2(val)                                        \
	(((uint32_t)(((uint32_t)(val)) << CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_RESUME_CORE2_SHIFT)) & \
	 CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_RESUME_CORE2_MASK)

/* W : Core 3 debug halt status flag bitfield:
  - 1'b0: running
  - 1'b1: halted
  R : Core 3 debug resume configuration bitfield:
  - 1'b0: stay halted
  - 1'b1: resume core 3 */
#define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_RESUME_CORE3_MASK  (0x8)
#define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_RESUME_CORE3_SHIFT (3)
#define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_RESUME_CORE3(val)                                        \
	(((uint32_t)(((uint32_t)(val)) << CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_RESUME_CORE3_SHIFT)) & \
	 CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_RESUME_CORE3_MASK)

/* W : Core 4 debug halt status flag bitfield:
  - 1'b0: running
  - 1'b1: halted
  R : Core 4 debug resume configuration bitfield:
  - 1'b0: stay halted
  - 1'b1: resume core 4 */
#define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_RESUME_CORE4_MASK  (0x10)
#define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_RESUME_CORE4_SHIFT (4)
#define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_RESUME_CORE4(val)                                        \
	(((uint32_t)(((uint32_t)(val)) << CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_RESUME_CORE4_SHIFT)) & \
	 CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_RESUME_CORE4_MASK)

/* W : Core 5 debug halt status flag bitfield:
  - 1'b0: running
  - 1'b1: halted
  R : Core 5 debug resume configuration bitfield:
  - 1'b0: stay halted
  - 1'b1: resume core 5 */
#define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_RESUME_CORE5_MASK  (0x20)
#define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_RESUME_CORE5_SHIFT (5)
#define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_RESUME_CORE5(val)                                        \
	(((uint32_t)(((uint32_t)(val)) << CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_RESUME_CORE5_SHIFT)) & \
	 CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_RESUME_CORE5_MASK)

/* W : Core 6 debug halt status flag bitfield:
  - 1'b0: running
  - 1'b1: halted
  R : Core 6 debug resume configuration bitfield:
  - 1'b0: stay halted
  - 1'b1: resume core 6 */
#define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_RESUME_CORE6_MASK  (0x40)
#define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_RESUME_CORE6_SHIFT (6)
#define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_RESUME_CORE6(val)                                        \
	(((uint32_t)(((uint32_t)(val)) << CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_RESUME_CORE6_SHIFT)) & \
	 CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_RESUME_CORE6_MASK)

/* W : Core 7 debug halt status flag bitfield:
  - 1'b0: running
  - 1'b1: halted
  R : Core 7 debug resume configuration bitfield:
  - 1'b0: stay halted
  - 1'b1: resume core 7 */
#define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_RESUME_CORE7_MASK  (0x80)
#define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_RESUME_CORE7_SHIFT (7)
#define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_RESUME_CORE7(val)                                        \
	(((uint32_t)(((uint32_t)(val)) << CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_RESUME_CORE7_SHIFT)) & \
	 CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_RESUME_CORE7_MASK)


/*! @name DBG_HALT_MASK */
/* Core 0 debug halt mask bitfield. When bit is set, core will be part of mask group and stopped
 * when one of the members of the group stops. */
#define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE0_MASK  (0x1)
#define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE0_SHIFT (0)
#define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE0(val)                                                 \
	(((uint32_t)(((uint32_t)(val)) << CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE0_SHIFT)) &          \
	 CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE0_MASK)

/* Core 1 debug halt mask bitfield. When bit is set, core will be part of mask group and stopped
 * when one of the members of the group stops. */
#define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE1_MASK  (0x2)
#define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE1_SHIFT (1)
#define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE1(val)                                                 \
	(((uint32_t)(((uint32_t)(val)) << CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE1_SHIFT)) &          \
	 CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE1_MASK)

/* Core 2 debug halt mask bitfield. When bit is set, core will be part of mask group and stopped
 * when one of the members of the group stops. */
#define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE2_MASK  (0x4)
#define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE2_SHIFT (2)
#define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE2(val)                                                 \
	(((uint32_t)(((uint32_t)(val)) << CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE2_SHIFT)) &          \
	 CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE2_MASK)

/* Core 3 debug halt mask bitfield. When bit is set, core will be part of mask group and stopped
 * when one of the members of the group stops. */
#define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE3_MASK  (0x8)
#define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE3_SHIFT (3)
#define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE3(val)                                                 \
	(((uint32_t)(((uint32_t)(val)) << CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE3_SHIFT)) &          \
	 CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE3_MASK)

/* Core 4 debug halt mask bitfield. When bit is set, core will be part of mask group and stopped
 * when one of the members of the group stops. */
#define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE4_MASK  (0x10)
#define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE4_SHIFT (4)
#define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE4(val)                                                 \
	(((uint32_t)(((uint32_t)(val)) << CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE4_SHIFT)) &          \
	 CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE4_MASK)

/* Core 5 debug halt mask bitfield. When bit is set, core will be part of mask group and stopped
 * when one of the members of the group stops. */
#define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE5_MASK  (0x20)
#define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE5_SHIFT (5)
#define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE5(val)                                                 \
	(((uint32_t)(((uint32_t)(val)) << CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE5_SHIFT)) &          \
	 CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE5_MASK)

/* Core 6 debug halt mask bitfield. When bit is set, core will be part of mask group and stopped
 * when one of the members of the group stops. */
#define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE6_MASK  (0x40)
#define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE6_SHIFT (6)
#define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE6(val)                                                 \
	(((uint32_t)(((uint32_t)(val)) << CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE6_SHIFT)) &          \
	 CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE6_MASK)

/* Core 7 debug halt mask bitfield. When bit is set, core will be part of mask group and stopped
 * when one of the members of the group stops. */
#define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE7_MASK  (0x80)
#define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE7_SHIFT (7)
#define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE7(val)                                                 \
	(((uint32_t)(((uint32_t)(val)) << CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE7_SHIFT)) &          \
	 CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE7_MASK)


/*! @name BOOT_ADDR0 */
/* Cluster core 0 boot address configuration bitfield. */
#define CLUSTER_CTRL_UNIT_BOOT_ADDR0_BA_MASK  (0xffffffff)
#define CLUSTER_CTRL_UNIT_BOOT_ADDR0_BA_SHIFT (0)
#define CLUSTER_CTRL_UNIT_BOOT_ADDR0_BA(val)                                                       \
	(((uint32_t)(((uint32_t)(val)) << CLUSTER_CTRL_UNIT_BOOT_ADDR0_BA_SHIFT)) &                \
	 CLUSTER_CTRL_UNIT_BOOT_ADDR0_BA_MASK)


/*! @name BOOT_ADDR1 */

/*! @name BOOT_ADDR2 */

/*! @name BOOT_ADDR3 */

/*! @name BOOT_ADDR4 */

/*! @name BOOT_ADDR5 */

/*! @name BOOT_ADDR6 */

/*! @name BOOT_ADDR7 */

/*! @name TCDM_ARB_POLICY_CH0 */
/* TCDM arbitration policy for cluster cores configuration bitfield:
  - 1'b0: fair round robin
  - 1'b1: fixed order */
#define CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH0_POL_MASK	(0x1)
#define CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH0_POL_SHIFT (0)
#define CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH0_POL(val)                                             \
	(((uint32_t)(((uint32_t)(val)) << CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH0_POL_SHIFT)) &      \
	 CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH0_POL_MASK)


/*! @name TCDM_ARB_POLICY_CH1 */
/* TCDM arbitration policy for DMA/HWCE configuration bitfield:
  - 1'b0: fair round robin
  - 1'b1: fixed order */
#define CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH1_POL_MASK	(0x1)
#define CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH1_POL_SHIFT (0)
#define CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH1_POL(val)                                             \
	(((uint32_t)(((uint32_t)(val)) << CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH1_POL_SHIFT)) &      \
	 CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH1_POL_MASK)


/*! @name TCDM_ARB_POLICY_CH0_REP */
/* TCDM arbitration policy for cluster cores configuration bitfield:
  - 1'b0: fair round robin
  - 1'b1: fixed order */
#define CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH0_REP_POL_MASK  (0x1)
#define CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH0_REP_POL_SHIFT (0)
#define CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH0_REP_POL(val)                                         \
	(((uint32_t)(((uint32_t)(val)) << CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH0_REP_POL_SHIFT)) &  \
	 CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH0_REP_POL_MASK)


/*! @name TCDM_ARB_POLICY_CH1_REP */
/* TCDM arbitration policy for DMA/HWCE configuration bitfield:
  - 1'b0: fair round robin
  - 1'b1: fixed order */
#define CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH1_REP_POL_MASK  (0x1)
#define CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH1_REP_POL_SHIFT (0)
#define CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH1_REP_POL(val)                                         \
	(((uint32_t)(((uint32_t)(val)) << CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH1_REP_POL_SHIFT)) &  \
	 CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH1_REP_POL_MASK)


/*! Fetch enable. */
static inline void hal_cl_ctrl_fetch_enable(uint32_t device_id, uint32_t nb_core)
{
	uint32_t base = (uint32_t)cl_glob_ctrl_unit(device_id);
	uint32_t fetch = (((1 << nb_core) - 1) & ARCHI_CLUSTER_PE_MASK);
	hal_write32((volatile void *)(base + CL_CTRL_UNIT_FETCH_EN), fetch);
}

static inline void hal_cl_ctrl_fetch_mask_enable(uint32_t device_id, uint32_t core_mask)
{
	uint32_t base = (uint32_t)cl_glob_ctrl_unit(device_id);
	hal_write32((volatile void *)(base + CL_CTRL_UNIT_FETCH_EN), core_mask);
}


/*! ClockGate cluster. */
static inline void hal_cl_ctrl_clock_gate_enable(uint32_t device_id)
{
	uint32_t base = (uint32_t)cl_glob_ctrl_unit(device_id);
	uint32_t cg = (CLUSTER_CTRL_UNIT_CLOCK_GATE_EN(1));
	hal_write32((volatile void *)(base + CL_CTRL_UNIT_CLOCK_GATE), cg);
}

static inline void hal_cl_ctrl_clock_gate_disable(uint32_t device_id)
{
	uint32_t base = (uint32_t)cl_glob_ctrl_unit(device_id);
	uint32_t cg = (CLUSTER_CTRL_UNIT_CLOCK_GATE_EN(0));
	hal_write32((volatile void *)(base + CL_CTRL_UNIT_CLOCK_GATE), cg);
}


/*! Boot address. */
static inline void hal_cl_ctrl_boot_addr_set(uint32_t device_id, uint8_t core_id,
					     uint32_t boot_addr)
{
	uint32_t base = (uint32_t)cl_glob_ctrl_unit(device_id);
	uint32_t offset = (CL_CTRL_UNIT_BOOT_ADDR_0 + (core_id << 2));
	hal_write32((volatile void *)(base + offset), boot_addr);
}

#endif /* __PMSIS_IMPLEM_HAL_CLUSTER_CLUSTER_CTRL_UNIT_H__ */
