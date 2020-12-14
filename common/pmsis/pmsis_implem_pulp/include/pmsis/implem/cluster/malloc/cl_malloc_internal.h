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

#ifndef __PI_CL_L2_MALLOC_H__
#define __PI_CL_L2_MALLOC_H__

#if defined(FEATURE_CLUSTER)

#include "pmsis.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

struct pi_cl_alloc_req_s
{
    void *chunk;
    int32_t size;
    uint8_t done;
    pi_task_t cb;
    uint8_t cid;
};

struct pi_cl_free_req_s
{
    void *chunk;
    int32_t size;
    uint8_t done;
    pi_task_t cb;
    uint8_t cid;
};

/*******************************************************************************
 * Driver data
 *****************************************************************************/

/*******************************************************************************
 * Function declaration
 ******************************************************************************/

/*******************************************************************************
 * Function implementation
 ******************************************************************************/

static inline void *pi_cl_l2_malloc_wait(pi_cl_alloc_req_t *req)
{
    cl_wait_task(&(req->done));
    return req->chunk;
}

static inline void pi_cl_l2_free_wait(pi_cl_free_req_t *req)
{
    cl_wait_task(&(req->done));
}

#endif  /* FEATURE_CLUSTER */

#endif  /* __PI_CL_L2_MALLOC_H__ */
