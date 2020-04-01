# SPDX-License-Identifier: Apache-2.0
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
include $(PROJ_ROOT)/RISC-V_PULP_Project/common/default_flags.mk

# rtos and pulp sources
include $(PROJ_ROOT)/RISC-V_PULP_Project/common/freertos_srcs.mk

# application name
PROG = queue

# application/user specific code
USER_SRCS = queue.c

# FreeRTOS.h
CPPFLAGS += $(addprefix -I, ".")

CPPFLAGS += -DportasmHANDLE_INTERRUPT=vSystemIrqHandler
CPPFLAGS += -DUSE_STDIO
# number of error check polls before the tests is conclueded as being successful
CPPFLAGS += -DQUEUE_CHECK_ITERATIONS=2

# compile, simulation and analysis targets
include $(PROJ_ROOT)/RISC-V_PULP_Project/common/default_targets.mk
