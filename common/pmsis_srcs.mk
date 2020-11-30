# Copyright 2020 ETH Zurich
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
# SPDX-License-Identifier: Apache-2.0
# Author: Robert Balas (balasr@iis.ee.ethz.ch)

PMSIS_ROOT = $(COMMON_ROOT)/pmsis
PMSIS_API_ROOT = $(COMMON_ROOT)/pmsis_api

CPPFLAGS += -I"$(PMSIS_ROOT)/pmsis_implem_pulp/include/"

PULP_SRCS += $(PMSIS_ROOT)/pmsis_implem_pulp/malloc/pulp_malloc.c

PULP_SRCS += $(PMSIS_ROOT)/pmsis_implem_pulp/drivers/fll/fll.c
PULP_SRCS += $(PMSIS_ROOT)/pmsis_implem_pulp/drivers/fc_event/fc_event.c

PULP_SRCS += $(PMSIS_ROOT)/pmsis_implem_pulp/drivers/udma/spi/spi.c
PULP_SRCS += $(PMSIS_ROOT)/pmsis_implem_pulp/drivers/udma/spi/spi_internal.c

PULP_SRCS += $(PMSIS_ROOT)/pmsis_implem_pulp/drivers/udma/uart/uart.c
PULP_SRCS += $(PMSIS_ROOT)/pmsis_implem_pulp/drivers/udma/uart/uart_internal.c

PULP_SRCS += $(PMSIS_ROOT)/pmsis_implem_pulp/drivers/gpio/gpio.c

PULP_SRCS += $(PMSIS_ROOT)/pmsis_implem_pulp/drivers/pad/pad.c

PULP_SRCS += $(PMSIS_ROOT)/pmsis_implem_pulp/drivers/timer/timer.c

# pmsis API
CPPFLAGS += -I"$(PMSIS_API_ROOT)/include"

# pmsis rtos specific
CPPFLAGS += -I"$(PMSIS_ROOT)/pmsis_rtos/include"

PULP_SRCS += $(PMSIS_ROOT)/pmsis_rtos/event_kernel/event_kernel.c
PULP_SRCS += $(PMSIS_ROOT)/pmsis_rtos/os/device.c
PULP_SRCS += $(PMSIS_ROOT)/pmsis_rtos/os/pmsis_task.c
PULP_SRCS += $(PMSIS_ROOT)/pmsis_rtos/pi_log.c

# for weird paths that start with "pmsis/"
CPPFLAGS += -I"$(COMMON_ROOT)"
# pmsis.h
CPPFLAGS += -I"$(PMSIS_ROOT)"
CPPFLAGS += -I"$(PMSIS_ROOT)/pmsis_implem_pulp/include/cores/TARGET_RISCV_32"

# pmsis backend abstraction layer
PULP_SRCS += $(PMSIS_ROOT)/backend/pmsis_backend_native_task_api.c

# TOOD: remove gap specific
CPPFLAGS += -I"$(COMMON_ROOT)/target/pulp/driver"
