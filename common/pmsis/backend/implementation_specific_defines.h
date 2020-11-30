/*
 * Copyright (c) 2019, GreenWaves Technologies, Inc.
 * Copyright (c) 2020, ETH Zurich
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

#ifndef __IMPLEMENTATION_SPECIFIC_DEFINES_H__
#define __IMPLEMENTATION_SPECIFIC_DEFINES_H__

#include "stdlib.h"
#include "string.h"

#if defined(__GAP8__)
#include "system_gap8.h"
#elif defined(__GAP9__)
#include "system_gap9.h"
#elif defined(__PULP__)
#include "system_pulp_ri5cy.h"
#else
#error "Target specific macro is not set. Recommended to use __PULP__."
#endif

#define __INC_TO_STRING(x) #x

#define PMSIS_TASK_EVENT_KERNEL_PRIORITY     2
#define DEFAULT_MALLOC_INC                   __INC_TO_STRING(pmsis/rtos/malloc/l2_malloc.h)

// default malloc for driver structs etc (might not be compatible with udma!)
/* TODO: rereoute to newlib malloc (?)*/
#ifdef __PULP__
#define pi_default_malloc(x)  malloc(x)
#define pi_default_free(x,y)  free(x)
#define pi_data_malloc(x)     malloc(x)
#define pi_data_free(x,y)     free(x)
#else
#error "__PULP__ not set: malloc not supported"
#define pi_default_malloc(x)  pmsis_l2_malloc(x)
#define pi_default_free(x,y)  pmsis_l2_malloc_free(x,y)
#define pi_data_malloc(x)     pmsis_l2_malloc(x)
#define pi_data_free(x,y)     pmsis_l2_malloc_free(x,y)
#endif

#define PI_TASK_IMPLEM                          \
    uint8_t destroy;

#define CLUSTER_TASK_IMPLEM

#endif  /* __IMPLEMENTATION_SPECIFIC_DEFINES_H__ */
