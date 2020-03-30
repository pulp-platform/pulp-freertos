# SPDX-License-Identifier: Apache-2.0
# Author: Robert Balas (balasr@iis.ee.ethz.ch)

# This makefile fragment defines default compile rules. It expect the default
# GNU Make C compiler flags set correctly (CFLAGS, CPPFLAGS, LDFLAGS), sensible
# values for $(RTOS_SRCS) $(PULP_SRCS) $(USER_SRCS), a well defined $(SIMDIR)
# and a $(PROG) name

ifndef SIMDIR
$(error "SIMDIR is empty. Simulation and analysis targets will not work")
endif

ifeq ($(strip $(PROG)),)
$(error "PROG is invalid or empty. Won't be able to compile.")
endif


# derived variables for compiling
SRCS = $(RTOS_SRCS) $(PULP_SRCS) $(USER_SRCS)
OBJS = $(addsuffix .o, $(basename $(SRCS))) # .S and .c replaced
DEPS = $(addsuffix .d, $(basename $(SRCS)))

# other possibly generated files
SU  = $(addsuffix .su, $(basename $(SRCS)))


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
# make sure we have sim directory
$(shell mkdir -p $(SIMDIR))

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
$(SIMDIR)/trace_fc_postproc.log: $(SIMDIR)/trace_core_1f_0.log
	$(PULPTRACE) $^ rtosdemo -o $@

analyze: $(SIMDIR)/trace_fc_postproc.log

# backup current simulation folder
.PHONY: backup
backup:
	@STAMP=sim-$$(git rev-parse --short HEAD)-$$(date +"%Y-%m-%d-%H-%M-%S"); \
	cp -r $(SIMDIR) $$STAMP; \
	cp rtosdemo rtosdemo.lst $$STAMP; \
	echo "generated backup $$STAMP";


.PHONY: clean
clean:
	rm -f $(OBJS) $(PROG) $(DEPS) $(SU) \
		$(PROG).hex $(PROG).lst $(PROG).siz \
		$(PROG).stim $(SIMDIR)/vectors/stim.txt
