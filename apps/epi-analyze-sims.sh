#!/usr/bin/env bash
set -e

usage () {
    echo "usage: epi-analyze-sims.sh simdir-list stats-dir"
    exit 1
}

[[ -f $1 ]] || usage
[[ -d $2 ]] || usage

simdir_list="$1"
stats_dir="$2"
readarray -t simdirs < "$simdir_list"

echo "Running analysis on the following simulations:"
for f in "${simdirs[@]}"; do
    echo "$f"
done
sleep 2

# clear records
printf "" > "${stats_dir}/analyzed"

for f in "${simdirs[@]}"; do
    printf "ANALYZING and FORKING $f\n"
    echo  "$stats_dir/$(basename -- $f)" >> "${stats_dir}/analyzed"
    ./epi-analyze-perf.sh "$f" "$stats_dir/$(basename -- $f)" &
done

wait < <(jobs -p)

exit 0
