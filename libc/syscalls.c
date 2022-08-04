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
 * SPDX-License-Identifier: Apache-2.0 OR ISC
 */

/* An extremely minimalist syscalls.c for newlib
 * Based on riscv newlib libgloss/riscv/sys_*.c
 *
 * Copyright 2019 Clifford Wolf
 * Copyright 2020 ETH Zurich
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH
 * REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,
 * INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */

/* C lib */
#include <sys/stat.h>
#include <sys/timeb.h>
#include <sys/time.h>
#include <newlib.h>
#include <unistd.h>
#include <errno.h>
#include <stdint.h>
#include <string.h>
#include <utime.h>

/* Drivers */
#include "memory_map.h"
#include "io.h"
#include "csr.h"
#include "apb_soc.h"
#include "stdout.h"

#if CONFIG_STDIO == STDIO_UART
#include "udma.h"
#include "udma_uart.h"
#endif

/* FreeRTOS */
#ifdef CONFIG_FREERTOS_KERNEL
#include "FreeRTOS.h"
#include "task.h"
#endif

#if !defined(configUSE_NEWLIB_REENTRANT) || (configUSE_NEWLIB_REENTRANT != 1)
#warning "configUSE_NEWLIB_REENTRANT is unset or zero. This setting \
is required for thread-safety of newlib sprintf, strtok, etc..."
#endif

#undef errno
extern int errno;

#define STDOUT_FILENO 1
#define STDERR_FILENO 2

/* It turns out that older newlib versions use different symbol names which goes
 * against newlib recommendations. Anyway this is fixed in later version.
 */
#if __NEWLIB__ <= 2 && __NEWLIB_MINOR__ <= 5
#define _sbrk	sbrk
#define _write	write
#define _close	close
#define _lseek	lseek
#define _read	read
#define _fstat	fstat
#define _isatty isatty
#define _kill   kill
#define _getpid getpid
#endif

ssize_t _write(int file, const void *ptr, size_t len);

void unimplemented_syscall()
{
	const char *p = "Unimplemented system call called!\n";
	_write(STDOUT_FILENO, p, strlen(p));
}

int nanosleep(const struct timespec *rqtp, struct timespec *rmtp)
{
	errno = ENOSYS;
	return -1;
}

int _access(const char *file, int mode)
{
	errno = ENOSYS;
	return -1;
}

int _chdir(const char *path)
{
	errno = ENOSYS;
	return -1;
}

int _chmod(const char *path, mode_t mode)
{
	errno = ENOSYS;
	return -1;
}

int _chown(const char *path, uid_t owner, gid_t group)
{
	errno = ENOSYS;
	return -1;
}

int _close(int file)
{
	return -1;
}

int _execve(const char *name, char *const argv[], char *const env[])
{
	errno = ENOMEM;
	return -1;
}

void _exit(int exit_status)
{
#if CONFIG_STDIO == STDIO_UART
	/* wait for the udma stdout to be emptied */
	while (readw((UDMA_UART(STDIO_UART_DEVICE_ID) +
		      UDMA_CHANNEL_TX_OFFSET +
		      UDMA_CHANNEL_CFG_OFFSET)) & UDMA_CORE_RX_CFG_EN_MASK) {
	}
	/* there is no way to check when the udma output fifo is empty so we
	 * just wait a few cycles */
	for (volatile int i = 0; i < 1024 * 3; i++)
	    ;
#endif
	writew(exit_status | (1 << APB_SOC_STATUS_EOC_BIT),
	       (uintptr_t)(APB_SOC_CTRL_ADDR + APB_SOC_CORESTATUS_OFFSET));
	for (;;)
	    asm volatile("wfi");
}

int _faccessat(int dirfd, const char *file, int mode, int flags)
{
	errno = ENOSYS;
	return -1;
}

int _fork(void)
{
	errno = EAGAIN;
	return -1;
}

int _fstat(int file, struct stat *st)
{
	st->st_mode = S_IFCHR;
	return 0;
	// errno = -ENOSYS;
	// return -1;
}

int _fstatat(int dirfd, const char *file, struct stat *st, int flags)
{
	errno = ENOSYS;
	return -1;
}

int _ftime(struct timeb *tp)
{
	errno = ENOSYS;
	return -1;
}

char *_getcwd(char *buf, size_t size)
{
	errno = -ENOSYS;
	return NULL;
}

int _getpid()
{
	return 1;
}

int _gettimeofday(struct timeval *tp, void *tzp)
{
	errno = -ENOSYS;
	return -1;
}

int _isatty(int file)
{
	return (file == STDOUT_FILENO);
}

int _kill(int pid, int sig)
{
	errno = EINVAL;
	return -1;
}

int _link(const char *old_name, const char *new_name)
{
	errno = EMLINK;
	return -1;
}

off_t _lseek(int file, off_t ptr, int dir)
{
	return 0;
}

int _lstat(const char *file, struct stat *st)
{
	errno = ENOSYS;
	return -1;
}

int _open(const char *name, int flags, int mode)
{
	return -1;
}

int _openat(int dirfd, const char *name, int flags, int mode)
{
	errno = ENOSYS;
	return -1;
}

ssize_t _read(int file, void *ptr, size_t len)
{
	return 0;
}

int _stat(const char *file, struct stat *st)
{
	st->st_mode = S_IFCHR;
	return 0;
	// errno = ENOSYS;
	// return -1;
}

long _sysconf(int name)
{
	return -1;
}

clock_t _times(struct tms *buf)
{
	return 0;
}

int _unlink(const char *name)
{
	errno = ENOENT;
	return -1;
}

int _utime(const char *path, const struct utimbuf *times)
{
	errno = ENOSYS;
	return -1;
}

int _wait(int *status)
{
	errno = ECHILD;
	return -1;
}

/* TODO: patch this out for cv32e40p */
static inline uint32_t pulp_core_id()
{
	/* encoding of mhartid: {21'b0, cluster_id_i[5:0], 1'b0, core_id_i[3:0]} */
	uint32_t mhartid = csr_read(CSR_MHARTID);
	return mhartid & 0x01f;
}

static inline uint32_t pulp_cluster_id()
{
	/* encoding of mhartid {21'b0, cluster_id_i[5:0], 1'b0, core_id_i[3:0]} */
	uint32_t mhartid = csr_read(CSR_MHARTID);
	return (mhartid >> 5) & 0x3f;
}

ssize_t _write(int file, const void *ptr, size_t len)
{
	/* fuse stout and stderr. remains to be seen if this is a good idea */
	if (file != STDOUT_FILENO && file != STDERR_FILENO) {
		errno = ENOSYS;
		return -1;
	}

#if CONFIG_STDIO == STDIO_FAKE
	const void *eptr = ptr + len;
	while (ptr != eptr)
		writew(*(unsigned char *)(ptr++),
		       (uintptr_t)(STDOUT_ADDR + STDOUT_PUTC_OFFSET +
				   (pulp_core_id() << 3) +
				   (pulp_cluster_id() << 7)));
	return (ssize_t)len;
#elif CONFIG_STDIO == STDIO_UART
	/* direct writes to uart udma*/
	static char copyout_buf[STDIO_UART_BUFSIZE] = {0};

	/* make sure we can issue a dma transfer, so wait until tx is done */
	while (readw((UDMA_UART(STDIO_UART_DEVICE_ID) +
		      UDMA_CHANNEL_TX_OFFSET +
		      UDMA_CHANNEL_CFG_OFFSET)) & UDMA_CORE_RX_CFG_EN_MASK) {
	}
	/* TODO: there is a race condition here when multiple threads start
	 * executing from here */

	/* copy to tmp buffer so that when we later functions don't clobber our
	 * buffer */
	memcpy(copyout_buf, ptr, len < STDIO_UART_BUFSIZE ? len : STDIO_UART_BUFSIZE);

	hal_uart_enqueue(STDIO_UART_DEVICE_ID, (uint32_t)copyout_buf, len,
			 UDMA_CORE_TX_CFG_EN_MASK |
			 REG_SET(UDMA_CORE_TX_CFG_DATASIZE, UDMA_CORE_CFG_DATASIZE_8),
			 TX_CHANNEL);

	/* We don't wait for the dma transfer to finish despite _write()'s
	 * blocking semantics. This should be ok since we are the only one's
	 * writing to uart */

	return len < STDIO_UART_BUFSIZE ? len : STDIO_UART_BUFSIZE;
#elif CONFIG_STDIO == STDIO_NULL
	/* just nop */
	return len;
#else
#error "CONFIG_STDIO is undefined"
#endif
}

extern char __heap_start[];
extern char __heap_end[];
static char *brk = __heap_start;

int _brk(void *addr)
{
	brk = addr;
	return 0;
}

void *_sbrk(ptrdiff_t incr)
{
	/* TODO: Check for stack collision by reading sp */
	char *old_brk = brk;

	if (brk + incr >= __heap_end) {
		errno = ENOMEM;
		return (void *)-1;
	}

	brk += incr;
	return old_brk;
}

void __malloc_lock(struct _reent *p)
{
	/* Make sure no mallocs inside ISRs */
	/* configASSERT(!xPortIsInsideInterrupt()); */
#ifdef CONFIG_FREERTOS_KERNEL
	vTaskSuspendAll();
#endif
}

void __malloc_unlock(struct _reent *p)
{
#ifdef CONFIG_FREERTOS_KERNEL
	(void)xTaskResumeAll();
#endif
}
