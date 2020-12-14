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

#include "pmsis_backend_native_types.h"
#include "pmsis.h"

/**
 * Kickoff the first "main" os task and scheduler
 */
int __os_native_kickoff(void *arg)
{
    BaseType_t xTask;
    TaskHandle_t xHandler0 = NULL;

    /* TODO: what is this weird logic ? */
    /* uint32_t stack_size = (uint32_t) MAIN_APP_STACK_SIZE; */
    /* stack_size /= sizeof(configSTACK_DEPTH_TYPE); */
    /* xTask = xTaskCreate(arg, "main", stack_size, */
    /*                     NULL, tskIDLE_PRIORITY + 1, &xHandler0); */
    xTask = xTaskCreate(arg, "main", ((unsigned short)configMINIMAL_STACK_SIZE),
                        NULL, tskIDLE_PRIORITY + 1, &xHandler0);

    if (xTask != pdPASS)
    {
        printf("main is NULL !\n");
        pmsis_exit(-4321);
    }

    __enable_irq();

    struct pmsis_event_kernel_wrap *wrap;
    /* TODO: blocks everything because priority too high */
    /* pmsis_event_kernel_init(&wrap, pmsis_event_kernel_main); */

    /* pmsis_event_set_default_scheduler(wrap); */

    hal_compiler_barrier();


    /* TODO: this is a quick hack to make the code run. Normally this should be
     * set in gap_io.c */
    uint8_t g_freertos_scheduler_started = 0;
    /* TODO: dito */
    SemaphoreHandle_t g_printf_mutex = NULL;


    /*
     * This should be used in case of printf via uart before scheduler has started.
     * Output will be on terminal instead of uart. After scheduler has started, output
     * will be via uart.
     */
    /* extern uint8_t g_freertos_scheduler_started; */

    g_freertos_scheduler_started = 1;

    /* extern SemaphoreHandle_t g_printf_mutex; */
    g_printf_mutex = xSemaphoreCreateMutex();
    if (g_printf_mutex == NULL)
    {
        printf("Error : printf mutex not created !\n", g_printf_mutex);
        pmsis_exit(-4322);
    }

    puts("pmsis_kickoff: starting scheduler\n");
    /* Start the kernel.  From here on, only tasks and interrupts will run. */
    vTaskStartScheduler();

    hal_compiler_barrier();

    /* Exit FreeRTOS */
    return 0;
}

void pi_time_wait_us(int time_us)
{
    /* Wait less than 1 ms. */
    if (time_us < 1000)
    {
        for (volatile int i=0; i<time_us; i++);
    }
    else
    {
        vTaskDelay((time_us/1000)/portTICK_PERIOD_MS);
    }
}
