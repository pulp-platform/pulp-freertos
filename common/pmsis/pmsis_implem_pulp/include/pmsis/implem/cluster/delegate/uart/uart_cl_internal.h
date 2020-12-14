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

#if defined(FEATURE_CLUSTER)

#ifndef __PMSIS_CLUSTER_DRIVERS_DELEGATE_UART_UART_CL_INTERNAL_H__
#define __PMSIS_CLUSTER_DRIVERS_DELEGATE_UART_UART_CL_INTERNAL_H__

#include "pmsis.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

struct pi_cl_uart_req_s
{
    pi_task_t req_cb;    /*!< Event task used to send the delegate transfer. */
    pi_task_t task_done; /*!< Event task used to check EOT. */
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

static inline void pi_cl_uart_write_wait(pi_cl_uart_req_t *req)
{
    pi_task_wait_on_no_mutex(&(req->task_done));
    hal_compiler_barrier();
}

static inline void pi_cl_uart_read_wait(pi_cl_uart_req_t *req)
{
    pi_task_wait_on_no_mutex(&(req->task_done));
    hal_compiler_barrier();
}


#endif  /* __PMSIS_CLUSTER_DRIVERS_DELEGATE_UART_UART_CL_INTERNAL_H__ */

#endif  /* FEATURE_CLUSTER */
