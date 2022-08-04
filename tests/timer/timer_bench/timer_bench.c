/*
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
 * Author: Robert Balas (balasr@iis.ee.ethz.ch)
 */

/*
 * A minimal pmsis example. Mainly used to test if we can link against pmsis.
 */

/* FreeRTOS kernel includes. */
#include <FreeRTOS.h>
#include <task.h>

/* c stdlib */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <inttypes.h>

/* system includes */
#include "system.h"
#include "io.h"
#include "timer.h"
#include "timer_hal.h"
#include "timer_irq.h"
#include "fll.h"
#include "irq.h"
#include "gpio.h"

/* pmsis */
#include "target.h"
#include "os.h"

//void vApplicationMallocFailedHook(void);
//void vApplicationIdleHook(void);
void vApplicationStackOverflowHook(TaskHandle_t pxTask, char *pcTaskName);
//void vApplicationTickHook(void);


int main(void) {

    system_init();

    int ret = 0;

    // Setup FC timer 0, LO
    timer_id_t timer0_id = TIMER_LO_ID;
    // Setup FC timer 1, HI
    timer_id_t timer1_id = TIMER_HI_ID;

    reset_timer_fc(timer0_id);
    reset_timer_fc(timer1_id);

    start_timer_fc(timer0_id);
    start_timer_fc(timer1_id);

    for (int i=0; i<100; i++) {
	
    }

    stop_timer_fc(timer0_id);
    stop_timer_fc(timer1_id);

    uint32_t time0_elapsed = get_timer_cnt_fc(timer0_id);
    uint32_t time1_elapsed = get_timer_cnt_fc(timer1_id);

    printf("Timer 0 elapsed: %d\nTimer 1 elapsed: %d\n", time0_elapsed, time1_elapsed);

    if (time0_elapsed == 0 || time1_elapsed == 0) {
	ret = 1;
    }

    return ret;
}

void vApplicationStackOverflowHook(TaskHandle_t pxTask, char *pcTaskName)
{
	(void)pcTaskName;
	(void)pxTask;

	taskDISABLE_INTERRUPTS();
	printf("error: stack overflow\n");
	__asm volatile("ebreak");
	for (;;)
		;
}
