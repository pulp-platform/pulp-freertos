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
PULP_SRCS += $(COMMON_ROOT)/chips/crt0.S $(COMMON_ROOT)/chips/vectors.S

ifeq ($(FREERTOS_CONFIG),PULP)
PULP_SRCS += $(COMMON_ROOT)/chips/system_pulpissimo_ri5cy.c
else ifeq ($(FREERTOS_CONFIG),PULPISSIMO)
PULP_SRCS += $(COMMON_ROOT)/chips/system_pulpissimo_ri5cy.c
else ifeq ($(FREERTOS_CONFIG),MRWOLF)
PULP_SRCS += $(COMMON_ROOT)/chips/system_pulpissimo_ri5cy.c
else ifeq ($(FREERTOS_CONFIG),GAP8)
$(error "GAP8 is not supported (yet)")
else
$(error "FREERTOS_CONFIG is unset. Run `source env/platform-you-want.sh' \
	from the freertos project's root folder.")
endif
# drivers and runtime
PULP_SRCS += $(addprefix $(COMMON_ROOT)/metal/, \
		fll.c timer_irq.c irq.c soc_eu.c gpio.c pinmux.c)
CPPFLAGS += -I"$(COMMON_ROOT)/metal/include"

# platform specific
CPPFLAGS += -I"$(COMMON_ROOT)/target_pulp/include"

