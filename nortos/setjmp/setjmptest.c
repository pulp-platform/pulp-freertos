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

/* Test if newlib's setjmp/longjmp works */

#include <stdlib.h>
#include <setjmp.h>
#include <stdio.h>

void eval(jmp_buf env, int prev_ret)
{
	int ret = 0;
	if (prev_ret == 0)
		ret = prev_ret;
	else
		ret = prev_ret + 1;

	printf("long jump, got result %d\n", ret);
	longjmp(env, ret);
}

int main(int argc, char *argv[])
{
	jmp_buf env;
	int ret = setjmp(env);

	printf("ret addr is 0x%08x\n", ret);

	if (ret > 1)
		return EXIT_SUCCESS;

	eval(env, ret);

	return EXIT_FAILURE;
}
