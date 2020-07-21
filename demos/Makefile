# Copyright 2019 ETH Zurich and University of Bologna
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

# Description: Makefile to build the blinky and other demo applications. Note
# that it supports the usual GNU Make implicit variables e.g. CC, CFLAGS,
# CPPFLAGS etc. Consult the GNU Make manual for move information about these.

# Author: Robert Balas (balasr@iis.ee.ethz.ch)

# Notes:
# Useful targets
# make all      Compile and link
# make run      Simulate SoC
# make backup   Record your simulation run
# make analyze  Run analysis scripts on the simulation result

# Important Variables
# PROG       Needs to be set to your executables name
# USER_SRCS  Add your source files here (use +=)
# CPPFLAGS   Add your include search paths and macro definitions (use +=)

# Adding common compile flags when using default_flags.mk.
# Compile options (passed to make) e.g. make NDEBUG=yes
# NDEBUG    Make release build
# LIBC      Link against libc
# LTO       Enable link time optimization
# SANITIZE  Enable gcc sanitizer for debugging memory access problems
# STACKDBG  Enable stack debugging information and warnings.
#           By default 1 KiB but can be changed with MAXSTACKSIZE=your_value


# indicate this repository's root folder
PROJ_ROOT = $(shell git rev-parse --show-toplevel)

# good defaults for many environment variables
include $(PROJ_ROOT)/common/default_flags.mk

# rtos and pulp sources
include $(PROJ_ROOT)/common/freertos_metal_srcs.mk

# application name
PROG = rtosdemo

# application/user specific code
USER_SRCS = main.c blinky_demo/main_blinky.c full_demo/RegTest.S \
	shared/Full/dynamic.c shared/Full/semtest.c \
	shared/Minimal/EventGroupsDemo.c shared/Minimal/TaskNotify.c \
	shared/Minimal/StreamBufferDemo.c shared/Minimal/StreamBufferInterrupt.c \
	shared/Full/print.c shared/Minimal/GenQTest.c \
	shared/Minimal/TimerDemo.c shared/Minimal/countsem.c \
	shared/Minimal/blocktim.c shared/Minimal/recmutex.c \
	shared/Minimal/AbortDelay.c shared/Minimal/death.c \
	shared/Minimal/MessageBufferDemo.c

# user headers
CPPFLAGS += $(addprefix -I, "shared/include" ".")

# point to irq handler
CPPFLAGS += -DportasmHANDLE_INTERRUPT=vSystemIrqHandler
CPPFLAGS += -DmainCREATE_SIMPLE_BLINKY_DEMO_ONLY=1

# compile, simulation and analysis targets
include $(PROJ_ROOT)/common/default_targets.mk