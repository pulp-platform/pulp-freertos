/*
 * Copyright (c) 2019 Yixiao Li <liyixiao7@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <kernel.h>
#include <t_syslog.h>
#include <t_stdlib.h>
#include "syssvc/serial.h"
#include "syssvc/syslog.h"
#include "kernel_cfg.h"

/*
 *  サービスコールのエラーのログ出力
 */
Inline void
svc_perror(const char *file, int_t line, const char *expr, ER ercd)
{
	if (ercd < 0) {
		t_perror(LOG_ERROR, file, line, expr, ercd);
	}
}

#define	SVC_PERROR(expr)	svc_perror(__FILE__, __LINE__, #expr, (expr))

#define ENABLE_PRINTF

static void
printf(const char *format, ...)
{
	va_list	ap;
	SYSLOG	logbuf;

	va_start(ap, format);
	tt_syslog(&logbuf, format, ap);
	va_end(ap);
	syslog_write(LOG_NOTICE, &logbuf);
}


#define main tm_basic_processing_test_main
#include "tm_basic_processing_test.c"
#undef main

#define main tm_cooperative_scheduling_test_main
#include "tm_cooperative_scheduling_test.c"
#undef main

#define main tm_interrupt_preemption_processing_test_main
#include "tm_interrupt_preemption_processing_test.c"
#undef main

#define main tm_interrupt_processing_test_main
#include "tm_interrupt_processing_test.c"
#undef main

#define main tm_memory_allocation_test_main
#include "tm_memory_allocation_test.c"
#undef main

#define main tm_message_processing_test_main
#include "tm_message_processing_test.c"
#undef main

#define main tm_preemptive_scheduling_test_main
#include "tm_preemptive_scheduling_test.c"
#undef main

#define main tm_synchronization_processing_test_main
#include "tm_synchronization_processing_test.c"
#undef main

static void *test_interrupt_handler;

void
tm_interrupt_handler()
{
	((void (*)(void)) test_interrupt_handler)();
}

/*
 *  メインタスク
 */
void
tm_main_task(intptr_t exinf)
{
	char	c;
	ER_UINT	ercd;

	SVC_PERROR(syslog_msk_log(LOG_UPTO(LOG_INFO), LOG_UPTO(LOG_EMERG)));
	syslog(LOG_NOTICE, "Thread-Metric Test Suite starts.");
	syslog(LOG_NOTICE, "1. Basic Processing Test");
	syslog(LOG_NOTICE, "2. Cooperative Scheduling Test");
	syslog(LOG_NOTICE, "3. Preemptive Scheduling Test");
	syslog(LOG_NOTICE, "4. Interrupt Processing Test");
	syslog(LOG_NOTICE, "5. Interrupt Preemption Processing Test");
	syslog(LOG_NOTICE, "6. Message Processing Test");
	syslog(LOG_NOTICE, "7. Synchronization Processing Test");
	syslog(LOG_NOTICE, "8. Memory Allocation Test");

	/*
	 *  シリアルポートの初期化
	 *
	 *  システムログタスクと同じシリアルポートを使う場合など，シリアル
	 *  ポートがオープン済みの場合にはここでE_OBJエラーになるが，支障は
	 *  ない．
	 */
	ercd = serial_opn_por(LOGTASK_PORTID);
	if (ercd < 0 && MERCD(ercd) != E_OBJ) {
		syslog(LOG_ERROR, "%s (%d) reported by `serial_opn_por'.",
									itron_strerror(ercd), SERCD(ercd));
	}
	SVC_PERROR(serial_ctl_por(LOGTASK_PORTID,
							(IOCTL_CRLF | IOCTL_FCSND | IOCTL_FCRCV)));

	SVC_PERROR(serial_rea_dat(LOGTASK_PORTID, &c, 1));

	switch (c) {
	case '1':
		syslog(LOG_NOTICE, "START: Basic Processing Test");
		tm_basic_processing_test_main();
		break;

	case '2':
		syslog(LOG_NOTICE, "START: Cooperative Scheduling Test");
		tm_cooperative_scheduling_test_main();
		break;

	case '3':
		syslog(LOG_NOTICE, "START: Preemptive Scheduling Test");
		tm_preemptive_scheduling_test_main();
		break;

	case '4':
		syslog(LOG_NOTICE, "START: Interrupt Processing Test");
		test_interrupt_handler = tm_interrupt_processing_handler;
		tm_interrupt_processing_test_main();
		break;

	case '5':
		syslog(LOG_NOTICE, "START: Interrupt Preemption Processing Test");
		test_interrupt_handler = tm_interrupt_preemption_handler;
		tm_interrupt_preemption_processing_test_main();
		break;

	case '6':
		syslog(LOG_NOTICE, "START: Message Processing Test");
		tm_message_processing_test_main();
		break;

	case '7':
		syslog(LOG_NOTICE, "START: Synchronization Processing Test");
		tm_synchronization_processing_test_main();
		break;

	case '8':
		syslog(LOG_NOTICE, "START: Memory Allocation Test");
		tm_memory_allocation_test_main();
		break;

	default:
		syslog(LOG_INFO, "Unknown command: '%c'.", c);
	}
}
