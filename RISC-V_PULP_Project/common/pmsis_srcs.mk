# SPDX-License-Identifier: Apache-2.0
# Author: Robert Balas (balasr@iis.ee.ethz.ch)

PMSIS_ROOT = $(COMMON_ROOT)/pmsis
PMSIS_API_ROOT = $(COMMON_ROOT)/pmsis_api

CPPFLAGS += -I"$(PMSIS_ROOT)/pmsis_implem_pulp/include/"

PULP_SRCS += $(PMSIS_ROOT)/pmsis_implem_pulp/drivers/udma/spi/spi.c
PULP_SRCS += $(PMSIS_ROOT)/pmsis_implem_pulp/drivers/udma/spi/spi_internal.c

# pmsis API
CPPFLAGS += -I"$(PMSIS_API_ROOT)/include"

# pmsis rtos specific
CPPFLAGS += -I"$(PMSIS_ROOT)/pmsis_rtos/include"

PULP_SRCS += $(PMSIS_ROOT)/pmsis_rtos/mem_slab/mem_slab.c

# for weird paths that start with "pmsis/"
CPPFLAGS += -I"$(COMMON_ROOT)"
# pmsis.h
CPPFLAGS += -I"$(PMSIS_ROOT)"
CPPFLAGS += -I"$(PMSIS_ROOT)/pmsis_implem_pulp/include/cores/TARGET_RISCV_32"

# TOOD: remove gap specific
CPPFLAGS += -I"$(COMMON_ROOT)/target_pulp/driver"
