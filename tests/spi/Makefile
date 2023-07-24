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

# For compile options check the README.md

# indicate this repository's root folder
PROJ_ROOT = $(shell git rev-parse --show-toplevel)

# good defaults for many environment variables
include $(PROJ_ROOT)/default_flags.mk

ifneq ($(DPI),yes)
$(error spi_test requires DPI=yes to work properly)
endif
# testbench configuration for spim_verif dpi
DPI_CONFIG=$(dir $(realpath $(firstword $(MAKEFILE_LIST))))rtl_config.json

# disable cluster
CONFIG_CLUSTER=n

# rtos, pulp and pmsis ources
include $(PROJ_ROOT)/default_srcs.mk

# application name
PROG = spi_test

# application/user specific code
USER_SRCS = spi_test.c

# FreeRTOS.h
CPPFLAGS += $(addprefix -I$(USER_DIR)/, ".")

CPPFLAGS += -DASYNC=0
CPPFLAGS += -DSPIM_ITF=0 -DSPIM_CS=1

# Uncomment to disable Additional reigsters (HW Loops)
#CPPFLAGS += -DportasmSKIP_ADDITIONAL_REGISTERS

# compile, simulation and analysis targets
include $(PROJ_ROOT)/default_targets.mk
