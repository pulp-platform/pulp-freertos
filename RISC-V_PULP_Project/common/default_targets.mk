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
ifdef interactive
	cd $(SIMDIR) && \
	export VSIM_RUNNER_FLAGS="+ENTRY_POINT=0x1c000880 -gLOAD_L2=JTAG \
		-dpicpppath $(CXX) -permit_unmatched_virtual_intf \
		-gBAUDRATE=115200" && \
	$(VSIM) -64 -c -do 'source $(VSIM_PATH)/tcl_files/config/run_and_exit.tcl' \
		-do 'source $(VSIM_PATH)/tcl_files/run.tcl;'
else
	cd $(SIMDIR) && \
	export VSIM_RUNNER_FLAGS="+ENTRY_POINT=0x1c000880 -gLOAD_L2=JTAG \
		-dpicpppath $(CXX) -permit_unmatched_virtual_intf \
		-gBAUDRATE=115200" && \
	$(VSIM) -64 -c -do 'source $(VSIM_PATH)/tcl_files/config/run_and_exit.tcl' \
		-do 'source $(VSIM_PATH)/tcl_files/run.tcl; run_and_exit;'
endif
endif

# analysis scripts
$(SIMDIR)/trace_fc_postproc.log: $(SIMDIR)/trace_core_1f_0.log
	$(PULPTRACE) $^ $(PROG) -o $@

analyze: $(SIMDIR)/trace_fc_postproc.log

$(SIMDIR)/trace_fc_compress.log: $(SIMDIR)/trace_fc_postproc.log
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

compress-trace: $(SIMDIR)/trace_fc_compress.log

# backup current simulation folder
.PHONY: backup
backup:
	@STAMP=sim-$$(git rev-parse --short HEAD)-$$(date +"%Y-%m-%d-%H-%M-%S"); \
	cp -r $(SIMDIR) $$STAMP; \
	cp $(PROG) $(PROG).lst $$STAMP; \
	echo "generated backup $$STAMP";


.PHONY: clean
clean:
	rm -f $(OBJS) $(PROG) $(DEPS) $(SU) \
		$(PROG).hex $(PROG).lst $(PROG).siz $(PROG).map \
		$(PROG).stim $(SIMDIR)/vectors/stim.txt
