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
# run make help for an updated overview

# Important Variables
# PROG       Needs to be set to your executables name
# USER_SRCS  Add your source files here (use +=)
# CPPFLAGS   Add your include search paths and macro definitions (use +=)

# For compile options check the README.md

# indicate this repository's root folder
# set some project specific path variables
ifndef FREERTOS_PROJ_ROOT
$(error "FREERTOS_PROJ_ROOT is unset. Run `source env/platform-you-want.sh' \
	from the freertos project's root folder.")
endif

# good defaults for many environment variables
include $(FREERTOS_PROJ_ROOT)/default_flags.mk

# rtos and pulp sources
include $(FREERTOS_PROJ_ROOT)/default_srcs.mk

# application name
PROG = blinky

# application/user specific code
USER_SRCS = main.c blinky_demo/main_blinky.c

# user headers
CPPFLAGS += $(addprefix -I$(USER_DIR)/, ".")

# point to irq handler
CPPFLAGS += -DportasmHANDLE_INTERRUPT=vSystemIrqHandler
CPPFLAGS += -DmainCREATE_SIMPLE_BLINKY_DEMO_ONLY=1

# compile, simulation and analysis targets
include $(FREERTOS_PROJ_ROOT)/default_targets.mk
