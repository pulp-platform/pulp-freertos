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
ifeq ($(CONFIG_FREERTOS_KERNEL),y)
dir := kernel

SRCS += $(dir)/event_groups.c
SRCS += $(dir)/list.c
SRCS += $(dir)/queue.c
SRCS += $(dir)/stream_buffer.c
SRCS += $(dir)/tasks.c
SRCS += $(dir)/timers.c
# RISC-V port files
SRCS += $(dir)/portable/GCC/RISC-V/port.c
SRCS += $(dir)/portable/GCC/RISC-V/portASM.S
# memory managment
SRCS += $(dir)/portable/MemMang/heap_3.c
# freertos macro
CV_CPPFLAGS += -DCONFIG_FREERTOS_KERNEL
# freertos generic headers
CV_CPPFLAGS += -I"$(FREERTOS_PROJ_ROOT)/$(dir)/include"
CV_CPPFLAGS += -I"$(FREERTOS_PROJ_ROOT)/$(dir)/portable/GCC/RISC-V"
# freertos header for assembler
ifeq ($(CONFIG_FREERTOS_CHIP_INCLUDE),)
$(error "CONFIG_FREERTOS_CHIP_INCLUDE is unset. Set to your target platform. See README.md.")
endif
CV_CPPFLAGS += -I"$(FREERTOS_PROJ_ROOT)/$(dir)/portable/GCC/RISC-V/chip_specific_extensions/$(CONFIG_FREERTOS_CHIP_INCLUDE)"
endif

# arch (RISC-V) specific
dir := target/arch
include $(FREERTOS_PROJ_ROOT)/$(dir)/makefile.mk

# c runtime and init
ifeq ($(CONFIG_USE_NEWLIB),y)
# syscall shims / implementation
dir := libc
include $(FREERTOS_PROJ_ROOT)/$(dir)/makefile.mk
endif

# metal drivers and runtime
# target dependend files
dir := target/$(CONFIG_TARGET)
include $(FREERTOS_PROJ_ROOT)/$(dir)/makefile.mk

# drivers
# TODO: currently the drivers are not properly decoupled from the kernel so we
# depend on the kernel until this is fixed
dir := drivers
ifeq ($(CONFIG_FREERTOS_KERNEL),y)
include $(FREERTOS_PROJ_ROOT)/$(dir)/makefile.mk
else
# TODO: this part of the workaround above
CV_CPPFLAGS += -I$(FREERTOS_PROJ_ROOT)/$(dir)/include
endif
