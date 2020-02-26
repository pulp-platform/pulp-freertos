/*
 * Copyright 2019 ETH Zurich and University of Bologna
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/* Description: Register I/O assembly. We don't trust volatile pointers.
 * Authors: Robert Balas (balasr@iis.ee.ethz.ch)
 */

#ifndef PULP_IO_H
#define PULP_IO_H

#include <stdint.h>

/* generic I/O write */
static inline void writeb(uint8_t val, volatile uint32_t *addr)
{
	asm volatile("sb %0, 0(%1)" : : "r"(val), "r"(addr));
}

static inline void writeh(uint16_t val, volatile uint32_t *addr)
{
	asm volatile("sh %0, 0(%1)" : : "r"(val), "r"(addr));
}

static inline void writew(uint32_t val, volatile uint32_t *addr)
{
	asm volatile("sw %0, 0(%1)" : : "r"(val), "r"(addr));
}

static inline void writed( uint64_t val, volatile uint32_t *addr )
{
	asm volatile( "sd %0, 0(%1)" : : "r"( val ), "r"( addr ) );
}

/* generic I/O read */
static inline uint8_t readb(const volatile uint32_t *addr)
{
	uint8_t val;

	asm volatile("lb %0, 0(%1)" : "=r"(val) : "r"(addr));
	return val;
}

static inline uint16_t readh(const volatile uint32_t *addr)
{
	uint16_t val;

	asm volatile("lh %0, 0(%1)" : "=r"(val) : "r"(addr));
	return val;
}

static inline uint32_t readw(const volatile uint32_t *addr)
{
	uint32_t val;

	asm volatile("lw %0, 0(%1)" : "=r"(val) : "r"(addr));
	return val;
}

static inline uint64_t readd( const volatile uint32_t *addr )
{
	uint64_t val;

	asm volatile( "ld %0, 0(%1)" : "=r"( val ) : "r"( addr ) );
	return val;
}
#endif
