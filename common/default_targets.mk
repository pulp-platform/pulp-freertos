# SPDX-License-Identifier: Apache-2.0
# Author: Robert Balas (balasr@iis.ee.ethz.ch)

# This makefile fragment defines default compile rules. It expect the default
# GNU Make C compiler flags set correctly (CFLAGS, CPPFLAGS, LDFLAGS), sensible
# values for $(RTOS_SRCS) $(PULP_SRCS) $(USER_SRCS), a well defined $(SIMDIR)
# and a $(PROG) name

ifndef SIMDIR
$(error "SIMDIR is empty. Simulation and analysis targets will not work")
endif

ifndef GVSIMDIR
$(error "GVSIMDIR is empty. Simulation and analysis targets will not work")
endif

ifeq ($(strip $(PROG)),)
$(error "PROG is invalid or empty. Won't be able to compile.")
endif

# make sure we compile by default
.DEFAULT_GOAL := all

# dpi specific build targets
# ifeq ($(DPI),yes)
include $(SUPPORT_ROOT)/support.mk
# endif

# derived variables for compiling
SRCS = $(RTOS_SRCS) $(PULP_SRCS) $(USER_SRCS)
OBJS = $(addsuffix .o, $(basename $(SRCS))) # .S and .c replaced
DEPS = $(addsuffix .d, $(basename $(SRCS)))

# other possibly generated files
SU  = $(addsuffix .su, $(basename $(SRCS)))


## Compile and link executable. Obeys standard GNU variables used by implicit rules.
all: $(PROG) $(PROG).stim misc-info

# %.d: %.c
#	@set -e; rm -f $@; \
#	$(CC) -M $(CPPFLAGS) $< > $@.$$$$; \
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

$(PROG).veri: $(PROG)
	@echo 'Invoking: GNU RISC-V Cross Create Verilog Memory Image'
	$(OBJCOPY) -O verilog $(PROG) $@
	@echo 'Finished building: $@'
	@echo ' '

$(PROG).lst: $(PROG)
	@echo 'Invoking: GNU RISC-V Cross Create Listing'
	$(OBJDUMP) --source --all-headers --demangle --line-numbers --wide --prefix-addresses \
		$(PROG) > $@
	@echo 'Finished building: $@'
	@echo ' '

$(PROG).siz: $(PROG)
	@echo 'Invoking: GNU RISC-V Cross Print Size'
	$(SIZE) --format=berkeley $<
	@echo 'Finished building: $@'
	@echo ' '

# simulator specific targets
# make sure we have sim directory
$(shell mkdir -p $(SIMDIR))
# make sure we have gvsim directory
$(shell mkdir -p $(GVSIMDIR))

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

$(SIMDIR)/preload:
	mkdir -p -- $@

$(SIMDIR)/preload/private_bank_0 $(SIMDIR)/preload/private_bank_1 \
$(SIMDIR)/preload/elf.veri &: $(PROG).veri
	$(MEMCONV) $(PROG).veri $(SIMDIR)/preload

# default vsim flags for simulation
VSIM_RUN_FLAGS = +ENTRY_POINT=0x1c000880 -gLOAD_L2=JTAG \
		-dpicpppath $(CXX) -permit_unmatched_virtual_intf \
		-gBAUDRATE=115200

# signal to simulator to preload the binary
ifdef preload
  VSIM_RUN_FLAGS += +preload=preload
  # add to pre-requisites for run target
  RUN_MORE = $(SIMDIR)/preload/elf.veri
endif

# vsim argument to enable dpi
ifeq ($(DPI),yes)
  VSIM_DPI = $(addprefix -sv_lib ,$(basename $(DPI_LIBS)))
  ifeq ($(strip $(DPI_CONFIG)),)
  $(warning "DPI_CONFIG is unset, but requested DPI sim. Using default rtl_config.json")
  DPI_CONFIG = "rtl_config.json"
  endif
else
  DPI_CONFIG = "NONE"
endif

VSIM_RUN_FLAGS += -gCONFIG_FILE=$(DPI_CONFIG)

.PHONY:
## Run RTL simulation. Append gui=1 or interactive=1 for vsim gui or vsim shell respectively
run-sim: run

.PHONY: run
run: $(SIMDIR)/modelsim.ini $(SIMDIR)/boot $(SIMDIR)/tcl_files \
	$(SIMDIR)/waves $(SIMDIR)/vectors/stim.txt \
	$(SIMDIR)/stdout $(SIMDIR)/fs \
	$(DPI_LIBS) $(RUN_MORE)
ifndef VSIM_PATH
	$(error "VSIM_PATH is not set. Make sure your ran `source setup/vsim.sh` \
	in your PULP/PULPissimo repository")
endif
	cp $(PROG) $(SIMDIR)
	cp $(PROG).lst $(SIMDIR)
	cp $(PROG).map $(SIMDIR)
	if [[ -f $(PROG).veri ]]; then cp $(PROG).veri $(SIMDIR); fi;
ifdef gui
	cd $(SIMDIR) && \
	export LD_LIBRARY_PATH="$(SUPPORT_LIB_DIR)" && \
	export VSIM_RUNNER_FLAGS="$(VSIM_RUN_FLAGS) $(VSIM_DPI) $(VSIM_ARGS)" && \
	export VOPT_ACC_ENA="YES" && \
	$(VSIM) -64 -do 'source $(VSIM_PATH)/tcl_files/config/run_and_exit.tcl' \
		-do 'source $(VSIM_PATH)/tcl_files/run.tcl; ' $(VSIM_ARGS)
else
ifdef interactive
	cd $(SIMDIR) && \
	export LD_LIBRARY_PATH="$(SUPPORT_LIB_DIR)" && \
	export VSIM_RUNNER_FLAGS="$(VSIM_RUN_FLAGS) $(VSIM_DPI) $(VSIM_ARGS)" && \
	$(VSIM) -64 -c -do 'source $(VSIM_PATH)/tcl_files/config/run_and_exit.tcl' \
		-do 'source $(VSIM_PATH)/tcl_files/run.tcl;' $(VSIM_ARGS)
else
	cd $(SIMDIR) && \
	export LD_LIBRARY_PATH="$(SUPPORT_LIB_DIR)" && \
	export VSIM_RUNNER_FLAGS="$(VSIM_RUN_FLAGS) $(VSIM_DPI) $(VSIM_ARGS)" && \
	$(VSIM) -64 -c -do 'source $(VSIM_PATH)/tcl_files/config/run_and_exit.tcl' \
		-do 'source $(VSIM_PATH)/tcl_files/run.tcl; run_and_exit;' $(VSIM_ARGS)
endif
endif

GVSOC=$(SUPPORT_ROOT)/egvsoc.sh
.PHONY: run-gvsoc
## Run simulation using gvsoc
run-gvsoc: $(GVSIMDIR) gvsoc
	cp $(PROG) $(GVSIMDIR)
	cp $(PROG).lst $(GVSIMDIR)
	cp $(PROG).map $(GVSIMDIR)
	PULP_RISCV_GCC_TOOLCHAIN=$(PULP_RISCV_GCC_TOOLCHAIN) \
	$(GVSOC) --config-file=pulp@config_file=chips/pulp/pulp.json --platform=gvsoc \
		--dir=$(CURDIR)/$(GVSIMDIR) --binary=$(PROG) prepare run $(GVSIM_ARGS)

# analysis scripts
$(SIMDIR)/trace_%_postproc.log: $(SIMDIR)/trace_core_%.log
	$(PULPTRACE) --time --stats --cycles $^ $(PROG) -o $@

## Symbolize simulation instruction trace log using the the original executable
trace-symbolize: $(SIMDIR)/trace_1f_0_postproc.log

$(SIMDIR)/trace_%_simplify.log: $(SIMDIR)/trace_%_postproc.log
	sed -e '/: <_start>/a [...]' -e '1,/: <_start>/d' \
		-e '/: <malloc+0x[a-f0-9]\+>/d' -e '/: <malloc>/a [...]' \
		-e '/: <_malloc_r+0x[a-f0-9]\+>/d' -e '/: <_malloc_r>/a [...]' \
		-e '/: <memset+0x[a-f0-9]\+>/d' -e '/: <memset>/a [...]' \
		-e '/: <__malloc_lock+0x[a-f0-9]\+>/d' -e '/: <__malloc_lock>/a [...]' \
		-e '/: <_sbrk+0x[a-f0-9]\+>/d' -e '/: <_sbrk>/a [...]' \
		-e '/: <_sbrk_r+0x[a-f0-9]\+>/d' -e '/: <_sbrk_r>/a [...]' \
		-e '/: <__sfp+0x[a-f0-9]\+>/d' -e '/: <__sfp>/a [...]' \
		-e '/: <__sinit+0x[a-f0-9]\+>/d' -e '/: <__sinit>/a [...]' \
		-e '/: <std+0x[a-f0-9]\+>/d' -e '/: <std>/a [...]' \
		-e '/: <puts+0x[a-f0-9]\+>/d' -e '/: <puts>/a [...]' \
		-e '/: <_puts_r+0x[a-f0-9]\+>/d' -e '/: <_puts_r>/a [...]' \
		-e '/: <_write+0x[a-f0-9]\+>/d' -e '/: <_write>/a [...]' \
		-e '/: <_write_r+0x[a-f0-9]\+>/d' -e '/: <_write_r>/a [...]' \
		-e '/: <__swrite+0x[a-f0-9]\+>/d' -e '/: <__swrite>/a [...]' \
		-e '/: <__sflush_r+0x[a-f0-9]\+>/d' -e '/: <__sflush_r>/a [...]' \
		-e '/: <_fflush_r+0x[a-f0-9]\+>/d' -e '/: <_fflush_r>/a [...]' \
		-e '/: <__swbuf_r+0x[a-f0-9]\+>/d' -e '/: <__swbuf_r>/a [...]' \
		-e '/: <iprintf+0x[a-f0-9]\+>/d' -e '/: <iprintf>/a [...]' \
		-e '/: <_vfiprintf_r+0x[a-f0-9]\+>/d' -e '/: <_vfiprintf_r>/a [...]' \
		-e '/: <_printf_i+0x[a-f0-9]\+>/d' -e '/: <_printf_i>/a [...]' \
		-e '/: <_printf_common+0x[a-f0-9]\+>/d' -e '/: <_printf_common>/a [...]' \
		-e '/: <__sfputc_r+0x[a-f0-9]\+>/d' -e '/: <__sfputc_r>/a [...]' \
		-e '/: <__sfputs_r+0x[a-f0-9]\+>/d' -e '/: <__sfputs_r>/a [...]' \
		-e '/: <memchr+0x[a-f0-9]\+>/d' -e '/: <memchr>/a [...]' \
		-e '/: <__smakebuf_r+0x[a-f0-9]\+>/d' -e '/: <__smakebuf_r>/a [...]' \
		-e '/: <__swsetup_r+0x[a-f0-9]\+>/d' -e '/: <__swsetup_r>/a [...]' \
		$^ > $@
## Simplify simulation instruction trace log. This collapses some function calls.
trace-simplify: $(SIMDIR)/trace_1f_0_simplify.log

.PHONY: backup
## Backup current simulation folder
backup:
	@STAMP=sim-$$(git rev-parse --short HEAD)-$$(date +"%Y-%m-%d-%H-%M-%S"); \
	cp -r $(SIMDIR) $$STAMP; \
	cp $(PROG) $(PROG).lst $$STAMP; \
	echo "generated backup $$STAMP";

.PHONY: clean
## Clean object files
clean:
	rm -f $(OBJS) $(PROG) $(DEPS) $(SU) \
		$(PROG).hex $(PROG).lst $(PROG).siz $(PROG).map $(PROG).veri \
		$(PROG).stim $(SIMDIR)/vectors/stim.txt

.PHONY: distclean
## Clean object files and all support dependencies
distclean: clean
	rm -rf $(SUPPORT_ROOT)/install/*

.PHONY: show-config
## Show current configuration
show-config:
	@printf "Project settings:\n"
	@printf "FREERTOS_PROJ_ROOT=$(FREERTOS_PROJ_ROOT)\n"
	@printf "FREERTOS_CONFIG_FAMILY=$(FREERTOS_CONFIG)\n"
	@printf "\n"
	@printf "Compiler settings:\n"
	@printf "CC=$(CC)\n"
	@printf "CFLAGS=\"$(CFLAGS)\"\n"
	@printf "CPPFLAGS=\"$(CPPFLAGS)\"\n"
	@printf "\n"
	@printf "Assembler settings:\n"
	@printf "AS=$(CC)\n"
	@printf "ASFLAGS=\"$(ASFLAGS)\"\n"
	@printf "\n"
	@printf "Linker settings:\n"
	@printf "LDFLAGS=\"$(LDFLAGS)\"\n"
	@printf "LDLIBS=\"$(LDLIBS)\"\n"

.PHONY: help
## Generate help overview
help: Makefile
	@printf "Available targets\n\n"
	@awk '/^[a-zA-Z\-\_0-9]+:/ { \
		helpMessage = match(lastLine, /^## (.*)/); \
		if (helpMessage) { \
			helpCommand = substr($$1, 0, index($$1, ":")-1); \
			helpMessage = substr(lastLine, RSTART + 3, RLENGTH); \
			printf "%-15s %s\n", helpCommand, helpMessage; \
		} \
	} \
	{ lastLine = $$0 }' $(MAKEFILE_LIST)
