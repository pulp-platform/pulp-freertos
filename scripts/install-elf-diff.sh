#!/usr/bin/env bash
[[ ! -d elf_diff ]] || { echo "elf_diff is already downloaded. Aborting."; exit 1; }

git clone git@github.com:bluewww/elf_diff.git
patch -p1 -d elf_diff/ < elf-diff-py3.patch
