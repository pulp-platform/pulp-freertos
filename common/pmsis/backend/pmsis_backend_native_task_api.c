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
