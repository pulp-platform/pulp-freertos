# SPDX-License-Identifier: Apache-2.0
# Author: Robert Balas (balasr@iis.ee.ethz.ch)

# expects RTOS_ROOT to point to the FreeRTOS distribution root
# and COMMON_ROOT to the driver folder

# general OS
RTOS_SRCS = $(addprefix $(RTOS_ROOT)/Source/,\
		event_groups.c list.c queue.c stream_buffer.c tasks.c timers.c)
# RISC-V port files
RTOS_SRCS += $(addprefix $(RTOS_ROOT)/Source/portable/GCC/RISC-V/,\
		port.c portASM.S)
# memory managment
RTOS_SRCS += $(addprefix $(RTOS_ROOT)/Source/portable/MemMang/,\
		heap_3.c)
# freertos generic headers
CPPFLAGS += $(addprefix -I, \
		"$(RTOS_ROOT)/Source/include" \
		"$(RTOS_ROOT)/Source/portable/GCC/RISC-V")
# freertos header for assembler
CPPFLAGS  += -I"$(RTOS_ROOT)/Source/portable/GCC/RISC-V/chip_specific_extensions/PULPissimo"

# c runtime and init
ifneq ($(LIBC),no)
PULP_SRCS += $(COMMON_ROOT)/libc/syscalls.c # syscall shims / implementation
endif
PULP_SRCS += $(COMMON_ROOT)/crt0.S $(COMMON_ROOT)/vectors.S
# drivers and runtime
PULP_SRCS += $(addprefix $(COMMON_ROOT)/, \
		fll.c timer_irq.c irq.c soc_eu.c gpio.c pinmux.c)
# TODO: hardcoded target_pulp path
PULP_SRCS += $(addprefix $(COMMON_ROOT)/target_pulp/, \
		system_pulp_ri5cy.c)
CPPFLAGS += -I"$(COMMON_ROOT)/target_pulp/include"
# driver headers
CPPFLAGS += -I"$(COMMON_ROOT)/include"
