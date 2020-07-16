/* SPDX-License-Identifier: Apache-2.0
 * Author: https://www.embecosm.com/appnotes/ean9/ean9-howto-newlib-1.0.html#chap_newlib
 */

/* Test if newlib's setjmp/longjmp works */

#include <stdlib.h>
#include <setjmp.h>
#include <stdio.h>

void testit(jmp_buf env, int prev_res)
{
	int res = (0 == prev_res) ? prev_res : prev_res + 1;

	printf("Long jumping with result %d\n", res);
	longjmp(env, res);
}

int main(int argc, char *argv[])
{
	jmp_buf env;

	int res = setjmp(env);

	printf("res = 0x%08x\n", res);

	if (res > 1) {
		return EXIT_SUCCESS;
	}

	testit(env, res);

	return 256; /* We should never actually get here */
}
