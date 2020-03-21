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
# LTO       Enable link time optimization
# SANITIZE  Enable gcc sanitizer for debugging memory access problems
# STACKDBG  Enable stack debugging information and warnings

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

CFLAGS = \
	-march=rv32imc -mabi=ilp32 -msmall-data-limit=8 -mno-save-restore \
	-fsigned-char -ffunction-sections -fdata-sections \
	-std=gnu11 \
	-Wall -Wextra -Wshadow -Wformat=2 -Wundef -Wconversion

ASFLAGS = \
	-march=rv32imc -mabi=ilp32 -msmall-data-limit=8 -mno-save-restore \
	-fsigned-char -ffunction-sections -fdata-sections \
	-x assembler-with-cpp \
	-DportasmHANDLE_INTERRUPT=SystemIrqHandler

CPPFLAGS = -DNOT_MSCC_STDIO_THRU_CORE_UART_APB -DSYS_CLK_FREQ=66000000 \
			-DmainCREATE_SIMPLE_BLINKY_DEMO_ONLY=1

LDFLAGS	= -T link.ld -nostartfiles -Wl,--gc-sections --specs=nano.specs \
				-Wl,-Map,$@.map # -Wl,--print-gc-sections
LDLIBS  =

# check if we want a release build
ifndef NDEBUG
CFLAGS 	+= -Og -g3
ASFLAGS += -Og -g3
else
CFLAGS	 += -Os -g3
CPPFLAGS += -DNDEBUG
ASFLAGS  += -Os -g3
endif

# stack debugging information
ifdef STACKDBG
CFLAGS += -fstack-usage -Wstack-usage
endif

# check if we want to debug with memory sanitiszers
ifdef SANITIZE
CFLAGS += -fsanitize=address -fsanitize=undefined -fsanitize=leak
endif

# link time optimization
# note that the gnu manpage recommends passing the optimization flags used at
# compile time also to the linker when using LTO (weird!) which is why we have
# CFLAGS in the linker target
ifdef LTO
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
		heap_4.c)

# c runtime and init
BOOT_SRCS = crt0.S vectors.S

# drivers and runtime
DRIVER_SRCS += common/system_pulpissimo_ri5cy.c common/fll.c common/timer_irq.c
DRIVER_SRCS += common/irq.c common/soc_eu.c

# application/user specific code
#USER_SRCS = main.c full_demo/main_full.c full_demo/RegTest.S blinky_demo/main_blinky.c
USER_SRCS = main.c blinky_demo/main_blinky.c

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

ASFLAGS  += -I"$(RTOS_ROOT)/Source/portable/GCC/RISC-V/chip_specific_extensions/PULPissimo"

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
	$(CC) $(ASFLAGS) -c -o "$@" "$<"
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

.PHONY: run
run: $(SIMDIR)/modelsim.ini $(SIMDIR)/boot $(SIMDIR)/tcl_files \
	$(SIMDIR)/waves $(SIMDIR)/vectors/stim.txt
ifndef VSIM_PATH
	$(error "VSIM_PATH is not set. Make sure your ran `source setup/vsim.sh` \
	in your PULP/PULPissimo repository")
endif
ifdef gui
	cd $(SIMDIR) && \
	export VSIM_RUNNER_FLAGS="+ENTRY_POINT=0x1c008080 -gLOAD_L2=JTAG \
		-dpicpppath $(CXX) -permit_unmatched_virtual_intf \
		-gBAUDRATE=115200" && \
	export VOPT_ACC_ENA="YES" && \
	$(VSIM) -64 -do 'source $(VSIM_PATH)/tcl_files/config/run_and_exit.tcl' \
		-do 'source $(VSIM_PATH)/tcl_files/run.tcl; '
else
	cd $(SIMDIR) && \
	export VSIM_RUNNER_FLAGS="+ENTRY_POINT=0x1c008080 -gLOAD_L2=JTAG \
		-dpicpppath $(CXX) -permit_unmatched_virtual_intf \
		-gBAUDRATE=115200" && \
	$(VSIM) -64 -c -do 'source $(VSIM_PATH)/tcl_files/config/run_and_exit.tcl' \
		-do 'source $(VSIM_PATH)/tcl_files/run.tcl; run_and_exit;'
endif

# analysis scripts
sim/trace_fc_postproc.log: sim/trace_core_1f_0.log rtosdemo
	scripts/pulptrace $^ -o $@

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
