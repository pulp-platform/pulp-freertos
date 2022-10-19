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

# Compile options (passed to make) e.g. make NDEBUG=yes
# NDEBUG    Make release build
# CONFIG_USE_NEWLIB Link against libc
# CONFIG_CC_LTO       Enable link time optimization
# CONFIG_CC_SANITIZE  Enable gcc sanitizer for debugging memory access problems
# CONFIG_CC_STACKDBG  Enable stack debugging information and warnings.
#           By default 1 KiB but can be changed with MAXSTACKSIZE=your_value

ifdef RISCV
CC      = $(RISCV)/bin/riscv32-unknown-elf-gcc
OBJCOPY = $(RISCV)/bin/riscv32-unknown-elf-objcopy
OBJDUMP = $(RISCV)/bin/riscv32-unknown-elf-objdump
SIZE    = $(RISCV)/bin/riscv32-unknown-elf-size
else
CC      = riscv32-unknown-elf-gcc
OBJCOPY = riscv32-unknown-elf-objcopy
OBJDUMP = riscv32-unknown-elf-objdump
SIZE    = riscv32-unknown-elf-size
endif

# set some project specific path variables
ifndef FREERTOS_PROJ_ROOT
$(error "FREERTOS_PROJ_ROOT is unset. Initialize it by calling \
`source env/desired-config.sh' in FreeRTOS' project root directory.")
endif

SCRIPTS_ROOT = $(FREERTOS_PROJ_ROOT)/scripts
SUPPORT_ROOT = $(FREERTOS_PROJ_ROOT)/support

# For out tree builds. We set a default for regular builds to make handling
# simpler
VPATH = .

# test if gcc is newer than version 8
GCC_GTEQ_8 = $(shell expr `$(CC) -dumpversion | cut -f1 -d.` \>= 8)

# CFLAGS defaults explained
#
# -std=gnu11
#
# This is a c11 project with gnu extension
#
# -mno-save-restore
#
# Do not use function calls for the prologue/epilogue of functions. This is
# debatable.
#
# -msmall-data-limit
#
# Limit data being put into the small data sections. Needs more exploration.
#
# -ffunction-sections -fdata-sections:
#
# Each function and data element will get its own section. This allows the
# linker to mark certain sections as unused which in turn will be deleted by
# feeding --gc-sections to the linker. Shrinks binary.
#
# -Wall -Wextra -Wshadow -Wformat=2 -Wundef -Wconversion
#
# Good set of warnings preventing some common bugs (integer promotion issues,
# shadowed variables, undefined macros etc.)
#
# -Og and -Os
#
# Debug builds are -Og. Gcc promises to make debugabble builds. Release builds
# are -Os which is basically -O2 withouth optimizations that causes code bloat.

# FIXME: NOTE to -march
# Compiler toolchain looks like this currently
# ~/.riscv/bin/riscv32-unknown-elf-gcc --print-multi-lib
# .;
# rv32i/ilp32;@march=rv32i@mabi=ilp32
# rv32im/ilp32;@march=rv32im@mabi=ilp32
# rv32iac/ilp32;@march=rv32iac@mabi=ilp32
# rv32imac/ilp32;@march=rv32imac@mabi=ilp32
# rv32imafc/ilp32f;@march=rv32imafc@mabi=ilp32f
# rv64imac/lp64;@march=rv64imac@mabi=lp64
# rv64imafdc/lp64d;@march=rv64imafdc@mabi=lp64d

# This unfortunately means that when we use -march=rv32imc then the libc code
# that will be linked is from -march=rv32im since that is the best fitting
# subset. This results in non compressed libc code. A quick fix is using
# -march=rv32imac but thats just weird since we don't really support the
# A-Extension in this PULP version.

# User controllable standard makeflags
CFLAGS = -Os -g3 -march=rv32imac -mabi=ilp32
CPPFLAGS =
LDFLAGS =
LDLIBS =
ASFLAGS = -Os -g3 -march=rv32imac -mabi=ilp32

# Builtin mandatory flags. Need to be simply expanded variables for appends in
# sub-makefiles to work correctly
CV_CFLAGS := \
	-msmall-data-limit=8 -mno-save-restore \
	-fsigned-char -ffunction-sections -fdata-sections \
	-std=gnu11 \
	-Wall -Wextra -Wshadow -Wformat=2 -Wundef -Wsign-conversion -Wno-unused-parameter

CV_ASFLAGS := \
	-msmall-data-limit=8 -mno-save-restore \
	-fsigned-char -ffunction-sections -fdata-sections \
	-x assembler-with-cpp

CV_CPPFLAGS :=

# note: linkerscript is included in target directory makefile.mk
CV_LDFLAGS := -nostartfiles -Wl,--gc-sections -Wl,-Map,memory.map # -Wl,--print-gc-sections

# check if we want a release build
ifeq ($(RELEASE),yes)
CV_CPPFLAGS += -DNDEBUG
endif

# do we need libc (by default yes)
ifeq ($(CONFIG_USE_NEWLIB),n)
$(warning Libc disabled. This setup is hard to get working and probably fails at the linker stage)
CV_LDFLAGS += -nolibc
else
# We want to link against newlib with where we provide our syscall
# implementation. Note we can't use --specs=nano.specs since that links against
# newlib and libgloss where the later provides already a syscall implementation
# (which is just forwarding the implementation with systemcalls). Actually we
# can use --specs=nano.specs and just provide your syscalls.c file. For the most
# part it works well; our implementation overwrites the one from libgloss, but
# when enabling LTO everything breaks apart because of duplicate symbols. So we
# do it the proper way: link everything by hand.

ifeq ($(GCC_GTEQ_8), 1)
# newer gcc
CV_LDFLAGS  += -nolibc -static
LDLIBS      += -lc_nano -lm_nano
else
# legacy link for older gcc (namely pulp-gcc)
CV_LDFLAGS  += -nostdlib -static
LDLIBS      += -lgcc -lc -lm -lgcc
endif

CV_CPPFLAGS    += -D__PULP_USE_LIBC
endif

# stack debugging information
ifeq ($(CONFIG_CC_STACKDBG),y)
CV_CFLAGS += -fstack-usage
ifeq ($(CONFIG_CC_MAXSTACKSIZE),)
CV_CFLAGS += -Wstack-usage=1024
else
CV_CFLAGS += -Wstack-usage=$(CONFIG_CC_MAXSTACKSIZE)
endif
endif

# check if we want to debug with memory sanitiszers
ifeq ($(CONFIG_CC_SANITIZE),y)
CV_CFLAGS += -fsanitize=address -fsanitize=undefined -fsanitize=leak
endif

# link time optimization
# note that the gnu manpage recommends passing the optimization flags used at
# compile time also to the linker when using LTO (weird!) which is why we have
# CFLAGS in the linker target
ifeq ($(CONFIG_CC_LTO),y)
CV_CFLAGS += -flto
endif

# use dpi during simulation
ifeq ($(DPI),yes)
DPI_LIBS += $(SUPPORT_ROOT)/install/lib/libpulpdpi.so
endif

# script paths
PLPSTIM   = $(SCRIPTS_ROOT)/pulpstim
PULPTRACE = $(SCRIPTS_ROOT)/pulptrace
MEMCONV   = $(SCRIPTS_ROOT)/mem.tcl

# simulation names and paths
VSIM   = vsim
SIMDIR = sim
GVSIMDIR = gvsim
PULP_RISCV_GCC_TOOLCHAIN=$(RISCV) # for gvsoc

# force SRCS to be a "simply expanded variable"
SRCS :=
