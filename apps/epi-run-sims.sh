#!/usr/bin/env bash

set -e

usage () {
    echo "Usage: $0 sim-dir sim-prefix sim-config "
    echo "  sim-dir     storage location for simulation results"
    echo "  sim-prefix  prefix of each simulation result directory"
    echo "  sim-config  configuration file that defines this simulation run"
}

[[ -n $1 ]] || { usage; echo "error: missing sim-dir parameter"; exit 1; }
[[ -n $2 ]] || { usage; echo "error: missing prefix parameter"; exit 1; }
[[ -n $3 ]] || { usage; echo "error: missing sim-config parameter"; exit 1; }

sim_dir="$1"
sim_prefix="$2"

sim_config="$3"
[[ -f $sim_config ]] || { usage; echo "error: sim-config doesn't exist"; exit 1; }

src_dir=$(realpath epi_power/Code)


# loading simulation configuration and sanitize
source "$sim_config"
[[ -n $opt ]] || { echo "error: sim-config doesn't set opt"; exit 1; }
[[ -n $systick_rate_hz ]] || { echo "error: sim-config doesn't set systick_rate_hz"; exit 1; }
# define TASKS_CONTROL_PERIOD_US (500) // Time in us //when change this also hange TELEMETRY_POLLING_FREQUENCY
[[ -n $period_ms ]] || { echo "error: sim-config doesn't set period_ms"; exit 1; }
# define MEASURE_N_ITERATION 20 // This is needed for both debug and Measure (DEFAULT)
[[ -n $measure_iterations ]] || { echo "error: sim-config doesn't set measure_iterations"; exit 1; }

[[ -n $RISCV ]] || { echo "error sim-config doesn't specify RISCV (compiler path)"; exit 1; }

# clear simulation records
printf "" > "${sim_dir}/simulations"

for p in "${period_ms[@]}"; do
    # give it time to compile
    cd "$src_dir"
    echo "compiling program"
    make clean all "opt1=$opt" \
	 CPPEXTRA="-DMEASURE_N_ITERATION=${measure_iterations} -DTASKS_CONTROL_PERIOD_US=${p} \
-DconfigTICK_RATE_HZ=${systick_rate_hz}"

    # define simulation directory
    this_simdir="${sim_dir}/${sim_prefix}_period_${p}_iter_${measure_iterations}"

    # record the simulation directory simplifying the analysis step
    echo "${this_simdir}" >> "${sim_dir}/simulations"

    # make sure we alredy generate the memorydump for fastsim before we fork of
    # and race to delete it
    make "$this_simdir/preload/elf.veri" SIMDIR="$this_simdir"

    # simulate asynchronously
    echo "forking off sim with period=${p}, measure_iterations=${measure_iterations}"
    # TODO: write sim opts
    cd "$src_dir"
    make run "$this_simdir/trace_1f_0_postproc.log" \
	 preload=yes SIMDIR="$this_simdir" &

    # hack: give some time to run-sim before make clean removes the binary
    sleep 4
done

wait
echo "all sims finished"
