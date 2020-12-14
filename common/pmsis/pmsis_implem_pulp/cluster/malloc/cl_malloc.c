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

#if (FEATURE_CLUSTER == 1)

#include "pmsis/implem/cluster/malloc/cl_malloc_internal.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Driver data
 *****************************************************************************/

/*******************************************************************************
 * Function declaration
 ******************************************************************************/

void __pi_cl_delegate_l2_alloc(void *arg);

void __pi_cl_delegate_l2_free(void *arg);

/*******************************************************************************
 * Function implementation
 ******************************************************************************/

void __pi_cl_delegate_l2_alloc(void *arg)
{
    pi_cl_alloc_req_t *req = (pi_cl_alloc_req_t *) arg;
    req->chunk = pmsis_l2_malloc(req->size);
    cl_notify_task_done(&(req->done), req->cid);
}

void __pi_cl_delegate_l2_free(void *arg)
{
    pi_cl_free_req_t *req = (pi_cl_free_req_t *) arg;
    pmsis_l2_malloc_free(req->chunk, req->size);
    req->chunk = NULL;
    cl_notify_task_done(&(req->done), req->cid);
}

/*******************************************************************************
 * API implementation
 ******************************************************************************/

void pi_cl_l2_malloc(int size, pi_cl_alloc_req_t *req)
{
    req->chunk = NULL;
    req->size = size;
    req->done = 0;
    req->cid = pi_cluster_id();
    /* Callback. */
    pi_task_callback_no_mutex(&(req->cb), __pi_cl_delegate_l2_alloc, req);
    pi_cl_send_task_to_fc(&(req->cb));
    hal_compiler_barrier();
}

void pi_cl_l2_free(void *chunk, int size, pi_cl_free_req_t *req)
{
    req->chunk = chunk;
    req->size = size;
    req->done = 0;
    req->cid = pi_cluster_id();
    /* Callback. */
    pi_task_callback_no_mutex(&(req->cb), __pi_cl_delegate_l2_free, req);
    pi_cl_send_task_to_fc(&(req->cb));
    hal_compiler_barrier();
}

#endif  /* (FEATURE_CLUSTER == 1) */
