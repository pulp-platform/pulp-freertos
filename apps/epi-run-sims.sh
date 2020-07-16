#!/usr/bin/env bash

set -e

usage () {
    echo "Usage: $0 [-o] simdir"
    echo "-o: Run with optimizations"
    exit
}

opt=no
while getopts ":o" o; do
    case "${o}" in
        o)
            opt=yes
            ;;
        *)
            ;;
    esac
done
shift $((OPTIND-1))

parent_sim_dir="$(dirname "$1")"
[[ -d $parent_sim_dir ]] || usage

# compiler
export RISCV=$HOME/.riscv32e
# pulp compiled rtl simulation
source $HOME/projects/pulp/setup/vsim.sh

epi_dir=$(realpath epi_power/Code)
sim_dir="/scratch/balasr/sim/0000-00-00/vanilla_dir"
sim_dir="$1"

# from EPI_Config.h
# define MEASURE_N_ITERATION 20 // This is needed for both debug and Measure (DEFAULT)
measure_iterations=20
# define TASKS_CONTROL_PERIOD_US (500) // Time in us //when change this also hange TELEMETRY_POLLING_FREQUENCY
periods=(500 450 400 350 300 250 200 180 170 160 150 140 130 120 110 100 90 80 70 60 50 40 30 20 10 5)

systick_rate=50

for p in "${periods[@]}"; do
    m=$measure_iterations
    t=$systick_rate
    # give it time to compile
    # TODO: opt level
    cd "$epi_dir"
    echo "compiling firmware"
    make clean all "opt1=$opt" CPPEXTRA="-DMEASURE_N_ITERATION=${m} -DTASKS_CONTROL_PERIOD_US=${p} -DconfigTICK_RATE_HZ=${t}"

    # define simulation directory
    this_simdir="${sim_dir}_period_${p}_iter_${m}"
    # make sure we alredy generate the memorydump for fastsim before we fork of
    # and race to delete it
    make "$this_simdir/preload/elf.veri" SIMDIR="$this_simdir"

    # simulate asynchronously
    echo "forking off sim with period=$p, measure_iterations=$m"
    # TODO: write sim opts
    cd "$epi_dir"
    make run "$this_simdir/trace_1f_0_postproc.log" \
	 preload=yes SIMDIR="$this_simdir" \
	 CPPEXTRA="-DMEASURE_N_ITERATION=${m} -DTASKS_CONTROL_PERIOD_US=${p} -DconfigTICK_RATE_HZ=${t}" &
done
