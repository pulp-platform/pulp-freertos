/*
 * Copyright (c) 2018, GreenWaves Technologies, Inc.
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

#include <stdlib.h>
#include "pmsis/implem/drivers/fll/fll.h"
/* TODO: abstract this */
#include "system_pulp_ri5cy.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/*
 * Fll control
 * FreqOut = Fref * Mult/2^(Div-1)
 * With Mult on 16 bits and Div on 4 bits
 */

/* Maximum Log2(DCO Frequency) */
#define FLL_LOG2_MAXDCO 29
/* Maximum Log2(Clok Divider) */
#define FLL_LOG2_MAXDIV 15
/* Log2(FLL_REF_CLK=32768) */
#define FLL_LOG2_REFCLK ARCHI_REF_CLOCK_LOG2
/* Maximum Log2(Multiplier) */
#define FLL_LOG2_MAXM (FLL_LOG2_MAXDCO - FLL_LOG2_REFCLK)

/* TODO: we are patching over these macros to make them independent of pulp-gcc */
#define __MAX(a, b)                                                            \
	({                                                                     \
		typeof(a) _a = (a);                                            \
		typeof(b) _b = (b);                                            \
		_a > _b ? _a : _b;                                             \
	})

#define __FL1(x) (31 - __builtin_clz((x)))

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/
static volatile uint32_t flls_frequency[FLL_NUM];

/*******************************************************************************
 * Code
 ******************************************************************************/
static uint32_t fll_get_mult_div_from_frequency(uint32_t freq, uint32_t *mult,
						uint32_t *div)
{
	unsigned int fref = FLL_REF_CLK;
	unsigned int Log2M = __FL1(freq) - __FL1(fref);
	unsigned int D = __MAX(1, (FLL_LOG2_MAXM - Log2M) >> 1);
	unsigned int M = (freq << D) / fref;
	unsigned int fres;

	fres = (fref * M + (1 << (D - 1))) >> D; /* Rounding */

	*mult = M;
	*div = D + 1;
	return fres;
}

static uint32_t fll_get_frequency_from_mult_div(uint32_t mult, uint32_t div)
{
	/* FreqOut = Fref * Mult/2^(Div-1)    With Mult on 16 bits and Div on 4 bits */
	uint32_t fref = FLL_REF_CLK;
	uint32_t fres = (div == 0) ? (fref * mult) : (fref * mult) >> (div - 1);
	return fres;
}

int pi_fll_set_frequency(fll_type_t which_fll, uint32_t frequency, int check)
{
	uint32_t val1, val2;
	uint32_t mult, div, mult_factor_diff;

	int irq = __disable_irq();

	/* Frequency calculation from theory */
	fll_get_mult_div_from_frequency(frequency, &mult, &div);

	/* Gain : 2-1 - 2-10 (0x2-0xB) */
	/* Return to close loop mode and give gain to feedback loop */
	val2 = FLL_CTRL_CONF2_LOOPGAIN(0x7) |
	       FLL_CTRL_CONF2_DEASSERT_CYCLES(0x10) |
	       FLL_CTRL_CONF2_ASSERT_CYCLES(0x10) |
	       FLL_CTRL_CONF2_LOCK_TOLERANCE(0x100) |
	       FLL_CTRL_CONF2_CONF_CLK_SEL(0x0) |
	       FLL_CTRL_CONF2_OPEN_LOOP(0x0) | FLL_CTRL_CONF2_DITHERING(0x1);

	FLL_CTRL[which_fll].FLL_CONF2 = val2;

	/* Configure register 1 */
	val1 = FLL_CTRL_CONF1_MODE(1) | FLL_CTRL_CONF1_MULTI_FACTOR(mult) |
	       FLL_CTRL_CONF1_CLK_OUT_DIV(div);

	FLL_CTRL[which_fll].FLL_CONF1 = val1;

	/* Check FLL converge by compare status register with multiply factor */
	do {
		mult_factor_diff = abs(FLL_CTRL[which_fll].FLL_STATUS - mult);
	} while (mult_factor_diff > 0x10);

	val2 = FLL_CTRL_CONF2_LOOPGAIN(0xB) |
	       FLL_CTRL_CONF2_DEASSERT_CYCLES(0x10) |
	       FLL_CTRL_CONF2_ASSERT_CYCLES(0x10) |
	       FLL_CTRL_CONF2_LOCK_TOLERANCE(0x100) |
	       FLL_CTRL_CONF2_CONF_CLK_SEL(0x0) |
	       FLL_CTRL_CONF2_OPEN_LOOP(0x0) | FLL_CTRL_CONF2_DITHERING(0x1);

	FLL_CTRL[which_fll].FLL_CONF2 = val2;

	if (which_fll == FLL_SOC)
		system_core_clock_update();

	flls_frequency[which_fll] = frequency;

	__restore_irq(irq);

	return frequency;
}

void pi_fll_init(fll_type_t which_fll, uint32_t ret_state)
{
	uint32_t reg1;

	if (ret_state) {
		pi_fll_get_frequency(which_fll, 1);
	} else {
		reg1 = FLL_CTRL[which_fll].FLL_CONF1;

		/*
		 * Don't set the gain and integrator in case it has already been
		 * set by the boot code as it totally blocks the fll on the RTL
		 * platform. The boot code is anyway setting the same
		 * configuration.
		 */
		if (!READ_FLL_CTRL_CONF1_MODE(reg1)) {
			/* set clock ref lock assert count */
			uint32_t reg2 = FLL_CTRL[which_fll].FLL_CONF2;
			reg2 &= ~FLL_CTRL_CONF2_ASSERT_CYCLES_MASK;
			reg2 |= FLL_CTRL_CONF2_ASSERT_CYCLES(0x6);
			reg2 &= ~FLL_CTRL_CONF2_LOCK_TOLERANCE_MASK;
			reg2 |= FLL_CTRL_CONF2_LOCK_TOLERANCE(0x50);
			FLL_CTRL[which_fll].FLL_CONF2 = reg2;

			/*
			 * In order to lock the fll faster, we first setup an
			 * approximated frequency while we are in open loop as
			 * it is set immediately. Then starting from this
			 * frequency, we should reach the target one in lock
			 * mode faster. We are in open loop, prime the fll
			 * forcing dco input, approx 70 MHz
			 */
			uint32_t regint = FLL_CTRL[which_fll].FLL_INTEGRATOR;
			regint &= ~FLL_CTRL_INTEGRATOR_INT_PART_MASK;
			regint |= FLL_CTRL_INTEGRATOR_INT_PART(332);
			FLL_CTRL[which_fll].FLL_INTEGRATOR = regint;

			/* Lock FLL */
			reg1 &= ~FLL_CTRL_CONF1_OUTPUT_LOCK_EN_MASK;
			reg1 |= FLL_CTRL_CONF1_OUTPUT_LOCK_EN(1);
			reg1 &= ~FLL_CTRL_CONF1_MODE_MASK;
			reg1 |= FLL_CTRL_CONF1_MODE(1);
			FLL_CTRL[which_fll].FLL_CONF1 = reg1;
		}

		/*
		 * In case the FLL frequency was set while it was off, update it
		 * immediately
		 */
		uint32_t freq = flls_frequency[which_fll];
		if (freq) {
			if (pi_fll_set_frequency(which_fll, freq, 0))
				abort();
		} else {
			freq = fll_get_frequency_from_mult_div(
				READ_FLL_CTRL_CONF1_MULTI_FACTOR(reg1),
				READ_FLL_CTRL_CONF1_CLK_OUT_DIV(reg1));
			flls_frequency[which_fll] = freq;
		}
		/* TODO: see if we need this
		if (pi_fll_set_frequency(which_fll, DEFAULT_SYSTEM_CLOCK, 0) ==
		    -1)
			exit(-1);
		*/
	}
}

int pi_fll_get_frequency(fll_type_t which_fll, uint8_t real)
{
	if (real) {
		/* Frequency calculation from real world */
		int real_freq = 0;
		real_freq = fll_get_frequency_from_mult_div(
			FLL_CTRL[which_fll].FLL_STATUS,
			READ_FLL_CTRL_CONF1_CLK_OUT_DIV(
				FLL_CTRL[which_fll].FLL_CONF1));

		/* Update Frequency */
		flls_frequency[which_fll] = real_freq;
	}
	return flls_frequency[which_fll];
}

void pi_fll_deinit(fll_type_t which_fll)
{
	flls_frequency[which_fll] = 0;
}
