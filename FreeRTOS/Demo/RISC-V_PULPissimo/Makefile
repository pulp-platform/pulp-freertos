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

# Compile options (passed to make) e.g. make NDEBUG=yes
# NDEBUG    Make release build
# LIBC      Link against libc
# LTO       Enable link time optimization
# SANITIZE  Enable gcc sanitizer for debugging memory access problems
# STACKDBG  Enable stack debugging information and warnings.
#           By default 1 KiB but can be changed with MAXSTACKSIZE=your_value

RISCV		?= $(HOME)/.riscv
RISCV_PREFIX	?= $(RISCV)/bin/riscv32-unknown-elf-
CC		= $(RISCV_PREFIX)gcc
OBJCOPY		= $(RISCV_PREFIX)objcopy
OBJDUMP		= $(RISCV_PREFIX)objdump
SIZE		= $(RISCV_PREFIX)size

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

CFLAGS = \
	-march=rv32imac -mabi=ilp32 -msmall-data-limit=8 -mno-save-restore \
	-fsigned-char -ffunction-sections -fdata-sections \
	-std=gnu11 \
	-Wall -Wextra -Wshadow -Wformat=2 -Wundef -Wconversion -Wno-unused-parameter

ASFLAGS = \
	-march=rv32imac -mabi=ilp32 -msmall-data-limit=8 -mno-save-restore \
	-fsigned-char -ffunction-sections -fdata-sections \
	-x assembler-with-cpp \
	-DportasmHANDLE_INTERRUPT=vSystemIrqHandler

CPPFLAGS = -DNOT_MSCC_STDIO_THRU_CORE_UART_APB -DSYS_CLK_FREQ=66000000 \
			-DmainCREATE_SIMPLE_BLINKY_DEMO_ONLY=1

LDFLAGS	= -T common/chips/pulpissimo/link.ld -nostartfiles -Wl,--gc-sections \
			-Wl,-Map,$@.map # -Wl,--print-gc-sections
LDLIBS  =

# check if we want a release build
ifneq ($(RELEASE),yes)
CFLAGS 	+= -Og -g3
ASFLAGS += -Og -g3
else
CFLAGS	 += -Os -g3
CPPFLAGS += -DNDEBUG
ASFLAGS  += -Os -g3
endif

# do we need libc (by default yes)
ifeq ($(LIBC),no)
$(warning Libc disabled. This setup is hard to get working and probably fails at the linker stage)
LDFLAGS += -nolibc
else
# We want to link against newlib with where we provide our syscall
# implementation. Note we can't use --specs=nano.specs since that links against
# newlib and libgloss where the later provides already a syscall implementation
# (which is just forwarding the implementation with systemcalls). Actually we
# can use --specs=nano.specs and just provide your syscalls.c file. For the most
# part it works well; our implementation overwrites the one from libgloss, but
# when enabling LTO everything breaks apart because of duplicate symbols. So we
# do it the proper way: link everything by hand.
LDFLAGS     += -nolibc -static
LDLIBS      += -lc_nano -lm_nano
CPPFLAGS    += -D__PULP_USE_LIBC
DRIVER_SRCS += common/libc/syscalls.c # syscall shims / implementation
endif

# stack debugging information
ifeq ($(STACKDBG),yes)
CFLAGS += -fstack-usage
ifeq ($(MAXSTACKSIZE),)
CFLAGS += -Wstack-usage=1024
else
CFLAGS += -Wstack-usage=$(MAXSTACKSIZE)
endif
endif

# check if we want to debug with memory sanitiszers
ifeq ($(SANITIZE),yes)
CFLAGS += -fsanitize=address -fsanitize=undefined -fsanitize=leak
endif

# link time optimization
# note that the gnu manpage recommends passing the optimization flags used at
# compile time also to the linker when using LTO (weird!) which is why we have
# CFLAGS in the linker target
ifeq ($(LTO),yes)
CFLAGS += -flto
endif

PROG = rtosdemo

# simulation
SIMDIR = sim
VSIM   = vsim

RTOS_ROOT = ../../../FreeRTOS

# general OS
RTOS_SRCS = $(addprefix $(RTOS_ROOT)/Source/,\
		event_groups.c list.c queue.c stream_buffer.c tasks.c timers.c)
# RISC-V port files
RTOS_SRCS += $(addprefix $(RTOS_ROOT)/Source/portable/GCC/RISC-V/,\
		port.c portASM.S)
# memory managment
RTOS_SRCS += $(addprefix $(RTOS_ROOT)/Source/portable/MemMang/,\
		heap_3.c)

# c runtime and init
BOOT_SRCS = common/crt0.S common/vectors.S

# drivers and runtime
DRIVER_SRCS += common/system_pulpissimo_ri5cy.c common/fll.c common/timer_irq.c
DRIVER_SRCS += common/irq.c common/soc_eu.c common/gpio.c common/pinmux.c

# application/user specific code
#USER_SRCS = main.c full_demo/main_full.c full_demo/RegTest.S blinky_demo/main_blinky.c
USER_SRCS = main.c blinky_demo/main_blinky.c full_demo/main_full.c full_demo/RegTest.S \
		../Common/Full/dynamic.c ../Common/Full/semtest.c \
		../Common/Minimal/EventGroupsDemo.c ../Common/Minimal/TaskNotify.c \
		../Common/Minimal/StreamBufferDemo.c ../Common/Minimal/StreamBufferInterrupt.c \
		../Common/Full/print.c ../Common/Minimal/GenQTest.c \
		../Common/Minimal/TimerDemo.c ../Common/Minimal/countsem.c \
		../Common/Minimal/blocktim.c ../Common/Minimal/recmutex.c \
		../Common/Minimal/AbortDelay.c ../Common/Minimal/death.c \
		../Common/Minimal/MessageBufferDemo.c

# derived variables for compiling
SRCS = $(RTOS_SRCS) $(BOOT_SRCS) $(DRIVER_SRCS) $(USER_SRCS)
OBJS = $(addsuffix .o, $(basename $(SRCS))) # .S and .c replaced
DEPS = $(addsuffix .d, $(basename $(SRCS)))

# other possibly generated files
SU   = $(addsuffix .su, $(basename $(SRCS)))

CPPFLAGS += $(addprefix -I, "$(RTOS_ROOT)/Source/include" \
		"$(RTOS_ROOT)/Demo/Common/include" \
		"$(RTOS_ROOT)/Source/portable/GCC/RISC-V" \
		"$(RTOS_ROOT)/Demo/RISC-V_PULPissimo" \
		"$(RTOS_ROOT)/Demo/RISC-V_PULPissimo/common/include")

# for assembler
CPPFLAGS  += -I"$(RTOS_ROOT)/Source/portable/GCC/RISC-V/chip_specific_extensions/PULPissimo"

PLPSTIM = scripts/pulpstim

all: $(PROG) $(PROG).stim misc-info

# %.d: %.c
#	@set -e; rm -f $@; \
#	$(CC) -M $(CPPFLAGS) $(INCLUDES) $< > $@.$$$$; \
#	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
#	rm -f $@.$$$$

# include $(SRCS:.c=.d)

%.o: %.S
	@echo 'Building file: $<'
	@echo 'Invoking: GNU RISC-V Cross Assembler'
	$(CC) $(ASFLAGS) $(CPPFLAGS) -c -o $@ $<
	@echo 'Finished building: $<'
	@echo ' '

%.o: %.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU RISC-V Cross C Compiler'
	$(CC) $(CFLAGS) $(CPPFLAGS) -c -o $@ $<
	@echo 'Finished building: $<'
	@echo ' '

$(PROG): $(OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: GNU RISC-V Cross C Linker'
	$(CC) $(CFLAGS) $(LDFLAGS) $^ $(LDLIBS) -o $@
	@echo 'Finished building target: $@'
	@echo ' '


# objdump, listing and size of binary
misc-info: $(PROG).hex $(PROG).lst $(PROG).siz

$(PROG).stim: $(PROG)
	@echo 'Invoking: plpstim script'
	$(PLPSTIM) -o $@ $<
	@echo 'Finished building: $@'
	@echo ' '

$(PROG).hex: $(PROG)
	@echo 'Invoking: GNU RISC-V Cross Create Flash Image'
	$(OBJCOPY) -O ihex $(PROG)  $@
	@echo 'Finished building: $@'
	@echo ' '

$(PROG).lst: $(PROG)
	@echo 'Invoking: GNU RISC-V Cross Create Listing'
	$(OBJDUMP) --source --all-headers --demangle --line-numbers --wide \
		$(PROG) > $@
	@echo 'Finished building: $@'
	@echo ' '

$(PROG).siz: $(PROG)
	@echo 'Invoking: GNU RISC-V Cross Print Size'
	$(SIZE) --format=berkeley $<
	@echo 'Finished building: $@'
	@echo ' '

# simulator specific targets
# creating symlink farm because PULP/PULPissimo relies on hardcoded paths
$(SIMDIR)/modelsim.ini:
	ln -s $(VSIM_PATH)/modelsim.ini $@

$(SIMDIR)/boot:
	ln -s $(VSIM_PATH)/boot $@

$(SIMDIR)/tcl_files:
	ln -s $(VSIM_PATH)/tcl_files $@

$(SIMDIR)/waves:
	ln -s $(VSIM_PATH)/waves $@

$(SIMDIR)/vectors/stim.txt: $(PROG).stim
	mkdir -p -- "$$(dirname $@)"
	ln -rs $^ $@

$(SIMDIR)/stdout:
	mkdir -p -- $@

$(SIMDIR)/fs:
	mkdir -p -- $@

.PHONY: run
run: $(SIMDIR)/modelsim.ini $(SIMDIR)/boot $(SIMDIR)/tcl_files \
	$(SIMDIR)/waves $(SIMDIR)/vectors/stim.txt \
	$(SIMDIR)/stdout $(SIMDIR)/fs
ifndef VSIM_PATH
	$(error "VSIM_PATH is not set. Make sure your ran `source setup/vsim.sh` \
	in your PULP/PULPissimo repository")
endif
ifdef gui
	cd $(SIMDIR) && \
	export VSIM_RUNNER_FLAGS="+ENTRY_POINT=0x1c000880 -gLOAD_L2=JTAG \
		-dpicpppath $(CXX) -permit_unmatched_virtual_intf \
		-gBAUDRATE=115200" && \
	export VOPT_ACC_ENA="YES" && \
	$(VSIM) -64 -do 'source $(VSIM_PATH)/tcl_files/config/run_and_exit.tcl' \
		-do 'source $(VSIM_PATH)/tcl_files/run.tcl; '
else
	cd $(SIMDIR) && \
	export VSIM_RUNNER_FLAGS="+ENTRY_POINT=0x1c000880 -gLOAD_L2=JTAG \
		-dpicpppath $(CXX) -permit_unmatched_virtual_intf \
		-gBAUDRATE=115200" && \
	$(VSIM) -64 -c -do 'source $(VSIM_PATH)/tcl_files/config/run_and_exit.tcl' \
		-do 'source $(VSIM_PATH)/tcl_files/run.tcl; run_and_exit;'
endif

# analysis scripts
sim/trace_fc_postproc.log: sim/trace_core_1f_0.log
	scripts/pulptrace $^ rtosdemo -o $@

analyze: sim/trace_fc_postproc.log

# backup current simulation folder
.PHONY: backup
backup:
	@STAMP=sim-$$(git rev-parse --short HEAD)-$$(date +"%Y-%m-%d-%H-%M-%S"); \
	cp -r sim/ $$STAMP; \
	cp rtosdemo rtosdemo.lst $$STAMP; \
	echo "generated backup $$STAMP";


.PHONY: clean
clean:
	rm -f $(OBJS) $(PROG) $(DEPS) $(SU) \
		$(PROG).hex $(PROG).lst $(PROG).siz \
		$(PROG).stim $(SIMDIR)/vectors/stim.txt
