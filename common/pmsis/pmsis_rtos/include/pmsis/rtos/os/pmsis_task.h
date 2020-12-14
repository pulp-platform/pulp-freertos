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

#ifndef __PI_RTOS_PMSIS_TASK_H__
#define __PI_RTOS_PMSIS_TASK_H__

#include "pmsis.h"
#include "pmsis/task.h"
#include "pmsis/rtos/rtos.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Function declaration
 ******************************************************************************/

/**
 * \brief Prepare a blocking event task.
 *
 * This function initializes an instance of event task.
 * A semaphore will be initialized.
 *
 * \param task           Pointer to event task.
 *
 * \return task This function returns the event task initialized.
 */
pi_task_t *__pi_task_block(pi_task_t *task);

/**
 * \brief Prepare an event task with callback.
 *
 * This function initializes an instance of event task.
 * This event task executes the callback given in argument.
 *
 * \param callback_task  Pointer to event task.
 * \param func           Callback function.
 * \param arg            Callback function argument.
 *
 * \return task This function returns the event task initialized.
 */
pi_task_t *__pi_task_callback(pi_task_t *callback_task,
                              void (*func)(void *), void *arg);

/**
 * \brief Wait on an event task.
 *
 * This function allows to wait on an event task until the event occurs.
 *
 * \param task           Pointer to event task.
 */
void __pi_task_wait_on(pi_task_t *task);

/**
 * \brief Push an event task.
 *
 * This function is used to push an event task to the event kernel.
 *
 * \param task           Pointer to event task.
 */
void __pi_task_push(pi_task_t *task);

/**
 * \brief Delete an event task.
 *
 * This function removes an event task.
 * It will delete the semaphore if it has been allocated.
 *
 * \param task           Pointer to event task.
 */
void __pi_task_destroy(pi_task_t *task);

/*******************************************************************************
 * API implementation
 ******************************************************************************/

static inline pi_task_t *pi_task_block(pi_task_t *task)
{
    return __pi_task_block(task);
}

static inline pi_task_t *pi_task_callback(pi_task_t *task,
                                          void (*callback)(void*), void *arg)
{
    return __pi_task_callback(task, callback, arg);
}

static inline void pi_task_wait_on(pi_task_t *task)
{
    __pi_task_wait_on(task);
}

static inline void pi_task_push(pi_task_t *task)
{
    __pi_task_push(task);
}

static inline void pi_task_destroy(pi_task_t *task)
{
    __pi_task_destroy(task);
}

#endif  /* __PI_RTOS_PMSIS_TASK_H__ */
