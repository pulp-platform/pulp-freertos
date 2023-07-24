# Copyright (C) 2020 ETH Zurich
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
#
# SPDX-License-Identifier: MIT
# Author: Robert Balas (balasr@iis.ee.ethz.ch)

# Description: Makefile to build the blinky and other demo applications. Note
# that it supports the usual GNU Make implicit variables e.g. CC, CFLAGS,
# CPPFLAGS etc. Consult the GNU Make manual for move information about these.

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

# indicate this repository's root folder
PROJ_ROOT = $(shell git rev-parse --show-toplevel)

# good defaults for many environment variables
include $(PROJ_ROOT)/default_flags.mk

# rtos and pulp sources
include $(PROJ_ROOT)/default_srcs.mk

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
	shared/Minimal/MessageBufferDemo.c \
	full_demo/main_full.c

CONFIG_CLUSTER=n

# user headers
CPPFLAGS += $(addprefix -I$(USER_DIR)/, "shared/include" ".")

# point to irq handler
CPPFLAGS += -DmainCREATE_SIMPLE_BLINKY_DEMO_ONLY=0

# compile, simulation and analysis targets
include $(PROJ_ROOT)/default_targets.mk
