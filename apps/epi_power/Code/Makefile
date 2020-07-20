# # User Test
# #------------------------------------
# APP = test
# APP_SRCS 	= ./Firmware.c ./EPI_task.c ./EPI_math.c ./measure.c ./print_float.c ./model_spi.c  ./simulation.c #./model_uart.c
# APP_CFLAGS      +=
# INC_PATH	+= ./

# # Uncomment to disable preemptive scheduling
# # NO_PREEMPTION	= true


# # Uncomment to add debug option
# #COMMON += -g

# PMSIS_OS = freertos
# BOARD_NAME ?= gapuino
# #io=host

# include $(GAP_SDK_HOME)/tools/rules/pmsis_rules.mk

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


# two big pulses: 400'000'000 ns
# indicate this repository's root folder
PROJ_ROOT = $(shell git rev-parse --show-toplevel)

# good defaults for many environment variables
include $(PROJ_ROOT)/common/default_flags.mk

# manually set CFLAGS to disable some warnings (-Wconversion)

# vanilla
CFLAGS = \
	-march=rv32imac -mabi=ilp32 -msmall-data-limit=8 -mno-save-restore \
	-fsigned-char -ffunction-sections -fdata-sections \
	-std=gnu11 \
	-Wall -Wextra -Wshadow -Wformat=2 -Wundef \
	-Wno-unused-parameter -Wno-unused-variable \
	-O3 -g3 \
	-DFEATURE_CLUSTER=0 -D__PULP__=1 -DNDEBUG

ASFLAGS = \
	-march=rv32imac -mabi=ilp32 -msmall-data-limit=8 -mno-save-restore \
	-fsigned-char -ffunction-sections -fdata-sections \
	-x assembler-with-cpp

ifeq ($(opt1),yes)
# opt1: emac, no hwloop regs, vectored context switch
CFLAGS = \
	-march=rv32emac -mabi=ilp32e -msmall-data-limit=8 -mno-save-restore \
	-fsigned-char -ffunction-sections -fdata-sections \
	-std=gnu11 \
	-Wall -Wextra -Wshadow -Wformat=2 -Wundef \
	-Wno-unused-parameter -Wno-unused-variable \
	-O3 -g3 \
	-DFEATURE_CLUSTER=0 -D__PULP__=1 -DNDEBUG \
	-DportasmSKIP_ADDITIONAL_REGISTERS

ASFLAGS = \
	-march=rv32emac -mabi=ilp32e -msmall-data-limit=8 -mno-save-restore \
	-fsigned-char -ffunction-sections -fdata-sections \
	-x assembler-with-cpp \
	-DportasmSKIP_ADDITIONAL_REGISTERS \
	-DPULP_FREERTOS_VECTORED_CONTEXT_SWITCH
endif

# rtos, pulp and pmsis sources
include $(PROJ_ROOT)/common/freertos_pmsis_srcs.mk

# application name
PROG = epi

# application/user specific code
USER_SRCS = ./Firmware.c ./EPI_task.c ./EPI_math.c ./measure.c \
	./print_float.c ./model_spi.c  ./simulation.c \
	./FreeRTOS_Config/FreeRTOS_util.c  #./model_uart.c

# FreeRTOS.h
CPPFLAGS += $(addprefix -I, ".")
CPPFLAGS += $(addprefix -I, "FreeRTOS_Config/")

CPPFLAGS += -DportasmHANDLE_INTERRUPT=vSystemIrqHandler
CPPFLAGS += -DMEASURE_ACTIVE
# abort sim after reading perf counters
CPPFLAGS += -D__PULP_FW_ABORT
CPPFLAGS += $(CPPEXTRA)

# compile, simulation and analysis targets
include $(PROJ_ROOT)/common/default_targets.mk
