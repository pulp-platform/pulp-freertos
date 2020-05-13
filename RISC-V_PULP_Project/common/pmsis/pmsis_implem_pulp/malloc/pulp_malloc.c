/*
* SPDX-License-Identifier: Apache-2.0
* Author: Robert Balas (balasr@iis.ee.ethz.ch)
*
* Malloc for PULP*. We just forward this is newlib's malloc.
*/
#include <stdlib.h>

void *pi_l2_malloc(int size)
{
	return malloc(size);
}

void pi_l2_free(void *chunk, int size)
{
	(void)size;
	free(chunk);
}
