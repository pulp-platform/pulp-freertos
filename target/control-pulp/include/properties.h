/*
 * Copyright (C) 2019 ETH Zurich, University of Bologna and GreenWaves
 * Technologies
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

#ifndef __PROPERTIES_H__
#define __PROPERTIES_H__


/* PULP defs */
#define PULP

/* variable system and fixed peripheral frequency */
#define ASIC_SYSTEM_CLOCK 600000000u /* 600 MhZ */
#define FPGA_SYSTEM_CLOCK 20000000u  /* 20 MhZ */

#define ASIC_PERIPH_CLOCK 100000000u /* 100 MhZ */
#define FPGA_PERIPH_CLOCK 10000000u  /* 10 MhZ */

#define ARCHI_FPGA_FREQUENCY 5000000

/* hardware modules */
#define ARCHI_NUM_TIMER 1
#define ARCHI_NUM_FLL	2

#define ARCHI_REF_CLOCK_LOG2 15
#define ARCHI_REF_CLOCK	     (1 << ARCHI_REF_CLOCK_LOG2)

#define ARCHI_NB_FLL 0

#define __RT_FLL_CL	2
#define __RT_FLL_PERIPH 1
#define __RT_FLL_FC	0

#define __RT_FREQ_DOMAIN_FC	0
#define __RT_FREQ_DOMAIN_CL	2
#define __RT_FREQ_DOMAIN_PERIPH 1
#define RT_FREQ_NB_DOMAIN	3

/* Cores & cluster */
/* FC */
#define ARCHI_FC_CID (32)

/* Cluster */
#define ARCHI_HAS_CLUSTER   (1)
#define ARCHI_CL_CID(id)    (id)
#define ARCHI_CLUSTER_NB_PE (8) /* Processing elements. */

/* Memories */
/* FC memory */
#define ARCHI_HAS_FC_TCDM  (1)
#define ARCHI_HAS_FC_ALIAS (1)
#define FC_TCDM_SIZE	   (0x00004000) /* 16kB. */

/* L2 memory */
#define ARCHI_HAS_L2_ALIAS (0)
#define L2_SHARED_SIZE	   (0x00080000) /* 512kB. */

/* L1 cluster memory */
#define ARCHI_HAS_CL_L1_ALIAS (1)
#define CL_L1_SIZE	      (0x00010000) /* 64kB. */

/* L1 cluster TS */
#define ARCHI_HAS_CL_L1_TS (1)


/* TODO: fix this table */
/* UDMA peripherals */
#define UDMA_HAS_SPIM	(1)
#define UDMA_HAS_HYPER	(0)
#define UDMA_HAS_UART	(1)
#define UDMA_HAS_I2C	(1)
#define UDMA_HAS_DMACPY (0)
#define UDMA_HAS_I2S	(0)
#define UDMA_HAS_CPI	(0)

/* TODO: fix this table */
/* Number of UDMA peripherals */
#define UDMA_NB_SPIM   (8)
#define UDMA_NB_HYPER  (0)
#define UDMA_NB_UART   (1)
#define UDMA_NB_I2C    (12)
/* TODO: i2c slaves */
#define UDMA_NB_DMACPY (0)
#define UDMA_NB_I2S    (0)
#define UDMA_NB_CPI    (0)

#define UDMA_NB_PERIPH (26)
/* Size of each UDMA peripheral */
#define UDMA_PERIPH_SIZE_LOG2 (7)
#define UDMA_PERIPH_SIZE      (1 << UDMA_PERIPH_SIZE_LOG2)

/* UDMA peripherals ID, this maps to PER_ID_* in udma_subsystem.sv */
#define UDMA_UART_ID(id) (0 + (id))
#define UDMA_SPIM_ID(id) (1 + (id))
#define UDMA_I2C_ID(id)	 (9 + (id))


/* Pads & GPIO. */
#define ARCHI_NB_PAD  (0)
#define ARCHI_NB_GPIO (128)

#endif /* __PROPERTIES_H__ */
