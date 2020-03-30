#pragma once

#define TM_MAIN_PRIORITY (TMIN_TPRI)
#define TM_INTNO         INT_SWINT2

void tm_main_task(intptr_t exinf);
void tm_thread_task(intptr_t exinf);
void tm_interrupt_handler();
