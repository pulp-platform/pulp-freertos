# SPDX-License-Identifier: Apache-2.0
# Author: Robert Balas (balasr@iis.ee.ethz.ch)

# Description: Makefile to build the blinky and other demo applications. Note
# that it supports the usual GNU Make implicit variables e.g. CC, CFLAGS,
# CPPFLAGS etc. Consult the GNU Make manual for move information about these.

# Notes:
# Useful targets
# run make help for an updated overview

# Important Variables
# PROG       Needs to be set to your executables name
# USER_SRCS  Add your source files here (use +=)
# CPPFLAGS   Add your include search paths and macro definitions (use +=)

# Adding common compile flags when using default_flags.mk.
# Compile options (passed to make) e.g. make NDEBUG=yes
# RELEASE   Make release build (default no)
# LIBC      Link against libc (default yes)
# LTO       Enable link time optimization (default no)
# SANITIZE  Enable gcc sanitizer for debugging memory access problems (default no)
# STACKDBG  Enable stack debugging information and warnings.
#           By default 1 KiB but can be changed with MAXSTACKSIZE=your_value


# indicate this repository's root folder
# set some project specific path variables
ifndef PROJ_ROOT
$(error "PROJ_ROOT is unset. Run source env/platform-you-want.sh \
	from the freertos project's root folder.")
endif

# good defaults for many environment variables
include $(PROJ_ROOT)/common/default_flags.mk

# rtos and pulp sources
include $(PROJ_ROOT)/common/freertos_metal_srcs.mk

# application name
PROG = blinky

# application/user specific code
USER_SRCS = main.c blinky_demo/main_blinky.c

# user headers
CPPFLAGS += $(addprefix -I, ".")

# point to irq handler
CPPFLAGS += -DportasmHANDLE_INTERRUPT=vSystemIrqHandler
CPPFLAGS += -DmainCREATE_SIMPLE_BLINKY_DEMO_ONLY=1

# compile, simulation and analysis targets
include $(PROJ_ROOT)/common/default_targets.mk
