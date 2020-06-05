/* SPDX-License-Identifier: Apache-2.0
 * Author: Robert Balas (balasr@iis.ee.ethz.ch)
 * Description: Write to the whole l2 address space
 * (skipping the two private banks)
 */

#include <stdlib.h>
#include <stdio.h>
#include "pulp_io.h"

int main(int argc, char *argv[])
{
	uint32_t val = 0;
	printf("memscan start\n");
	for (uint32_t addr = 0x1c010000; addr < 0x1c080000; addr += 4) {
		val += 1;
		writew(val, addr);
	}
	printf("memscan finished\n");
	return EXIT_SUCCESS;
}
