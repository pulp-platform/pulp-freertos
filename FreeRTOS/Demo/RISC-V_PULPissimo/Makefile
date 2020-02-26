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


RISCV			?= $(HOME)/.riscv
RISCV_PREFIX		?= $(RISCV)/bin/riscv32-unknown-elf-
CC			= $(RISCV_PREFIX)gcc
OBJCOPY			= $(RISCV_PREFIX)objcopy
OBJDUMP			= $(RISCV_PREFIX)objdump
SIZE			= $(RISCV_PREFIX)size
CFLAGS			+= \
				-std=gnu11 \
				-Wall -Wextra \
				-march=rv32im -msmall-data-limit=8 -mno-save-restore -O0 \
				-fsigned-char -ffunction-sections -fdata-sections
# check if we want a release build
ifndef NDEBUG
CFLAGS 			+= -g3
else
CFLAGS			+= -DNDEBUG
endif

# check if we want to debug with memory sanitiszers
ifdef SANITIZE
CFLAGS                  += -fsanitize=address -fsanitize=undefined -fsanitize=leak
endif

CPPFLAGS		= -DNOT_MSCC_STDIO_THRU_CORE_UART_APB -DSYS_CLK_FREQ=66000000 \
			-DmainCREATE_SIMPLE_BLINKY_DEMO_ONLY=1

LDFLAGS			=
LDLIBS			=

LINKERSCRIPT            = link.ld

PROG			= rtosdemo

# simulation
SIMDIR                  = sim
VSIM                    = vsim

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

CC_INCLUDES = $(addprefix -I, "$(RTOS_ROOT)/Source/include" \
		"$(RTOS_ROOT)/Demo/Common/include" \
		"$(RTOS_ROOT)/Source/portable/GCC/RISC-V" \
		"$(RTOS_ROOT)/Demo/RISC-V_PULPissimo" \
		"$(RTOS_ROOT)/Demo/RISC-V_PULPissimo/common/include")

AS_INCLUDES = -I"$(RTOS_ROOT)/Source/portable/GCC/RISC-V/chip_specific_extensions/PULPissimo"

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
	$(CC) -march=rv32im -msmall-data-limit=8 -mno-save-restore -O0 \
		-fsigned-char -ffunction-sections -fdata-sections  -g3 \
		-x assembler-with-cpp \
		-DportasmHANDLE_INTERRUPT=SystemIrqHandler \
		$(AS_INCLUDES) \
		-c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

%.o: %.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU RISC-V Cross C Compiler'
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $(CC_INCLUDES) -c -o $@ $<
	@echo 'Finished building: $<'
	@echo ' '

$(PROG): $(OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: GNU RISC-V Cross C Linker'
	$(CC) $(CFLAGS) $(CPPFLAGS) -T $(LINKERSCRIPT) \
		-nostartfiles -Xlinker --gc-sections -Wl,-Map,$(PROG).map \
		--specs=nano.specs $(CC_INCLUDES) -o $@ \
		$(LDFLAGS) $^ $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '


# objdump, listing and size of binary
misc-info: $(PROG).hex $(PROG).lst $(PROG).siz

$(PROG).stim: $(PROG)
	@echo 'Invoking: plpstim script'
	$(PLPSTIM) -o $@ $<
	@echo 'Finished'
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

# run
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


.PHONY: clean
clean:
	rm -f $(OBJS) $(PROG) $(DEPS) $(PROG).hex $(PROG).lst $(PROG).siz \
		$(PROG).stim $(SIMDIR)/vectors/stim.txt
