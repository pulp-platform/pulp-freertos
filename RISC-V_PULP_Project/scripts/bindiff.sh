#!/usr/bin/env bash
usage () {
    echo "Usage: bindiff elf-a elf-b"
    exit
}
[[ -n $RISCV ]] || { echo "RISCV environment variable unset. Please point it to your toolchain."; exit 1; }
[[ -f $1 ]] || usage
[[ -f $2 ]] || usage
scripts/elf_diff/bin/elf_diff --bin_dir "$RISCV/bin" --bin-prefix "riscv32-unknown-elf-" $1 $2
