/*
 * Copyright 2019 GreenWaves Technologies
 * Copyright 2020 ETH Zurich
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

#ifndef __PMSIS_H__
#define __PMSIS_H__

#include <stddef.h>
#include <inttypes.h>

/* Debug helper. */
#ifdef DEBUG
#define DEBUG_PRINTF printf
#else
#define DEBUG_PRINTF(...) ((void) 0)
#endif  /* DEBUG */

#if (!defined(HW_VERIF_ONLY))

/* Backend includes. */
#include "pmsis/backend/implementation_specific_defines.h"
#include "pmsis/targets/target.h"
/* pmsis_api includes. */
#include "pmsis/device.h"
#include "pmsis/task.h"
#include "pmsis_types.h"
#include "pmsis/rtos/rtos.h"
#include "pmsis/rtos/assert.h"
#include "pmsis/mem_slab.h"

/* PMSIS includes. */
#include "pmsis/implem/debug.h"
#include "pmsis/implem/drivers/drivers_data.h"
#include "pmsis/implem/drivers/drivers.h"
#include "pmsis/rtos/os/pmsis_task.h"
#include "pmsis/rtos/os/pmsis_freq.h"
#include "pmsis/rtos/os/os.h"
#if defined(FEATURE_CLUSTER)
#include "pmsis/implem/cluster/cluster.h"
#endif  /* FEATURE_CLUSTER */

#endif  /* HW_VERIF_ONLY */

/* Hal includes. */
#include "pmsis/implem/hal/hal.h"

#endif  /* __PMSIS_H__ */
