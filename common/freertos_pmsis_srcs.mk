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

# expects RTOS_ROOT to point to the FreeRTOS distribution root
# and COMMON_ROOT to the driver folder

# general OS
RTOS_SRCS = $(addprefix $(RTOS_ROOT)/,\
		event_groups.c list.c queue.c stream_buffer.c tasks.c timers.c)
# RISC-V port files
RTOS_SRCS += $(addprefix $(RTOS_ROOT)/portable/GCC/RISC-V/,\
		port.c portASM.S)
# memory managment
RTOS_SRCS += $(addprefix $(RTOS_ROOT)/portable/MemMang/,\
		heap_3.c)
# freertos generic headers
CPPFLAGS += $(addprefix -I, \
		"$(RTOS_ROOT)/include" \
		"$(RTOS_ROOT)/portable/GCC/RISC-V")
# freertos header for assembler
CPPFLAGS  += -I"$(RTOS_ROOT)/portable/GCC/RISC-V/chip_specific_extensions/PULPissimo"

# c runtime and init
ifneq ($(LIBC),no)
PULP_SRCS += $(COMMON_ROOT)/libc/syscalls.c # syscall shims / implementation
endif
PULP_SRCS += $(COMMON_ROOT)/target/crt0.S $(COMMON_ROOT)/target/vectors.S
# metal drivers and runtime
PULP_SRCS += $(addprefix $(COMMON_ROOT)/metal/, \
		fll.c timer_irq.c irq.c soc_eu.c gpio.c pinmux.c)
CPPFLAGS += -I"$(COMMON_ROOT)/metal/include"
# TODO: hardcoded target/pulp path
PULP_SRCS += $(addprefix $(COMMON_ROOT)/target/pulp/, \
		system_pulp_ri5cy.c)
CPPFLAGS += -I"$(COMMON_ROOT)/target/pulp/include"

include $(COMMON_ROOT)/pmsis_srcs.mk
