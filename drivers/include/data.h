/*
 * Copyright (C) 2019 GreenWaves Technologies
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

/*
 * Authors: Germain Haugou, GreenWaves Technologies (germain.haugou@greenwaves-technologies.com)
 */


#ifndef __DATA_H__
#define __DATA_H__

#ifndef LANGUAGE_ASSEMBLY

// We cannot use tiny attribute if we use a generic riscv toolchain or LLVM or we there is fc specific memeory (TCDM or L2)
#if (defined(ARCHI_HAS_FC_TCDM) || defined(ARCHI_HAS_L2_ALIAS)) && !defined(__LLVM__) && !defined(__RISCV_GENERIC__) && !defined(__PULP_TOOLCHAIN__)
#define FC_TINY_ATTRIBUTE __attribute__ ((tiny))
#else
#define FC_TINY_ATTRIBUTE
#endif

#define POS_HANDLER __attribute__((section(".text_l2")))
#define POS_TEXT_L2 __attribute__((section(".text_l2")))
#define PI_LOCAL_CODE POS_TEXT_L2
#define PI_TEXT_L2 POS_TEXT_L2

#define PI_FC_L1       __attribute__((section(".data_fc")))
#define PI_FC_L1_TINY  __attribute__((section(".data_tiny_fc"))) FC_TINY_ATTRIBUTE

#define PI_FC_TINY     PI_FC_L1_TINY

#define PI_L2 __attribute__((section(".l2_data")))

#if defined(ARCHI_HAS_L1_ALIAS) && !defined(__PULP_TOOLCHAIN__)
#define PI_CL_L1_TINY __attribute__ ((tiny)) __attribute__((section(".data_tiny_l1")))
#else
#define PI_CL_L1_TINY __attribute__((section(".data_tiny_l1")))
#endif

#define PI_CL_L1 __attribute__((section(".data_l1")))
#define PI_L1 PI_CL_L1

#endif

#endif
