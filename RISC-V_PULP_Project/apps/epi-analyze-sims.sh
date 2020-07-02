#!/usr/bin/env bash
usage () {
    echo "usage: epi-analyze-sims.sh simdir-collection"
    exit
}

# [[ -d $1 ]] || usage

# simdirs=$1

# example
# ./epi-analyze-perf.sh /scratch/balasr/sim/2020-06-11/vanilla_dir_period_30_iter_20/ /scratch/balasr/sim/2020-06-11/stats/vanilla_dir_period_30_iter_20/

# dirs=(vanilla_dir_period_100_iter_20 vanilla_dir_period_150_iter_20 vanilla_dir_period_200_iter_20 \
# vanilla_dir_period_20_iter_20 vanilla_dir_period_250_iter_20 vanilla_dir_period_300_iter_20 vanilla_dir_period_30_iter_20 \
# vanilla_dir_period_350_iter_20 vanilla_dir_period_400_iter_20 vanilla_dir_period_450_iter_20 vanilla_dir_period_500_iter_20 \
# vanilla_dir_period_50_iter_20)
# dirs=(vanilla_dir_period_40_iter_20 vanilla_dir_period_10_iter_20)
#dirs=(vanilla_dir_period_8_iter_20 vanilla_dir_period_7_iter_20 vanilla_dir_period_6_iter_20 vanilla_dir_period_5_iter_20)

# dirs=(opt1_dir_period_100_iter_20 opt1_dir_period_10_iter_20 opt1_dir_period_150_iter_20 \
# opt1_dir_period_200_iter_20 opt1_dir_period_20_iter_20 opt1_dir_period_250_iter_20 opt1_dir_period_300_iter_20 \
# opt1_dir_period_30_iter_20 opt1_dir_period_350_iter_20 opt1_dir_period_400_iter_20 opt1_dir_period_40_iter_20 \
# opt1_dir_period_450_iter_20 opt1_dir_period_500_iter_20 opt1_dir_period_50_iter_20)
# dirs=(opt1_dir_period_8_iter_20 opt1_dir_period_7_iter_20 opt1_dir_period_6_iter_20 opt1_dir_period_5_iter_20)


dirs=(opt1_period_100_iter_20 \
opt1_period_10_iter_20 \
opt1_period_150_iter_20 \
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
vanilla_period_100_iter_20 \
vanilla_period_10_iter_20 \
vanilla_period_150_iter_20 \
vanilla_period_200_iter_20 \
vanilla_period_20_iter_20 \
vanilla_period_250_iter_20 \
vanilla_period_300_iter_20 \
vanilla_period_30_iter_20 \
vanilla_period_350_iter_20 \
vanilla_period_400_iter_20 \
vanilla_period_40_iter_20 \
vanilla_period_450_iter_20 \
vanilla_period_500_iter_20 \
vanilla_period_50_iter_20 \
vanilla_period_5_iter_20)

for f in "${dirs[@]}"; do
    printf "\nANALYZING and FORKING $f\n"
    ./epi-analyze-perf.sh "/scratch/balasr/sim/2020-06-26/$f" "/scratch/balasr/sim/2020-06-26/stats/$f" &
done

wait < <(jobs -p)
