#!/usr/bin/env bash
usage () {
    echo "usage: epi-analyze-sims.sh simdir-collection"
    exit
}

# [[ -d $1 ]] || usage

# simdirs=$1

# example
# ./epi-analyze-perf.sh /scratch/balasr/sim/2020-06-11/vanilla_dir_period_30_iter_20/ /scratch/balasr/sim/2020-06-11/stats/vanilla_dir_period_30_iter_20/

# dirs=(opt1_period_100_iter_20 \
# opt1_period_10_iter_20 \
# opt1_period_150_iter_20 \
# opt1_period_200_iter_20 \
# opt1_period_20_iter_20 \
# opt1_period_250_iter_20 \
# opt1_period_300_iter_20 \
# opt1_period_30_iter_20 \
# opt1_period_350_iter_20 \
# opt1_period_400_iter_20 \
# opt1_period_40_iter_20 \
# opt1_period_450_iter_20 \
# opt1_period_500_iter_20 \
# opt1_period_50_iter_20 \
# opt1_period_5_iter_20 \
# vanilla_period_100_iter_20 \
# vanilla_period_10_iter_20 \
# vanilla_period_150_iter_20 \
# vanilla_period_200_iter_20 \
# vanilla_period_20_iter_20 \
# vanilla_period_250_iter_20 \
# vanilla_period_300_iter_20 \
# vanilla_period_30_iter_20 \
# vanilla_period_350_iter_20 \
# vanilla_period_400_iter_20 \
# vanilla_period_40_iter_20 \
# vanilla_period_450_iter_20 \
# vanilla_period_500_iter_20 \
# vanilla_period_50_iter_20 \
# vanilla_period_5_iter_20)

dirs=(opt1_period_100_iter_20 \
opt1_period_10_iter_20 \
opt1_period_110_iter_20 \
opt1_period_120_iter_20 \
opt1_period_130_iter_20 \
opt1_period_140_iter_20 \
opt1_period_150_iter_20 \
opt1_period_160_iter_20 \
opt1_period_170_iter_20 \
opt1_period_180_iter_20 \
opt1_period_200_iter_20 \
opt1_period_20_iter_20 \
opt1_period_250_iter_20 \
opt1_period_300_iter_20 \
opt1_period_30_iter_20 \
opt1_period_350_iter_20 \
opt1_period_400_iter_20 \
opt1_period_40_iter_20 \
opt1_period_450_iter_20 \
opt1_period_500_iter_20 \
opt1_period_50_iter_20 \
opt1_period_5_iter_20 \
opt1_period_60_iter_20 \
opt1_period_70_iter_20 \
opt1_period_80_iter_20 \
opt1_period_90_iter_20)


for f in "${dirs[@]}"; do
    printf "\nANALYZING and FORKING $f\n"
    ./epi-analyze-perf.sh "/scratch/balasr/sim/2020-07-07/$f" "/scratch/balasr/sim/2020-07-07/stats/$f" &
done

wait < <(jobs -p)
