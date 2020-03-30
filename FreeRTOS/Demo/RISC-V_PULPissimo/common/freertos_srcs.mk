# SPDX-License-Identifier: Apache-2.0
# Author: Robert Balas (balasr@iis.ee.ethz.ch)

# TODO: make this depend on source script and set sensible default with warning
# as of now this is a totally broken relative link that "just works"
RTOS_ROOT = ../../../FreeRTOS

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
PULP_SRCS += common/libc/syscalls.c # syscall shims / implementation
endif
PULP_SRCS += common/crt0.S common/vectors.S

# drivers and runtime
PULP_SRCS += common/system_pulpissimo_ri5cy.c common/fll.c common/timer_irq.c
PULP_SRCS += common/irq.c common/soc_eu.c common/gpio.c common/pinmux.c

# driver headers
CPPFLAGS += -I"$(RTOS_ROOT)/Demo/RISC-V_PULPissimo/common/include"
