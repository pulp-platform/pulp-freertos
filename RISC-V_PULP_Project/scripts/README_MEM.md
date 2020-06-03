# mem.tcl
Read, write and convert verilog style memory dumps. Mainly used with vsim and
PULP/PULPissimo. You use this to

- Convert verilog style memory dumps depending on your needs (addressing, word
  width etc.)
- Dump the contents of memories during simulation (finding heap corruption bugs etc.)
- Generate memory images from an executable to be preloaded by the simulator.
  Speeds up simulation time.

## Why TCL
Simply because most EDA tools use TCL as scripting language. This makes
integrating memory manipulation easier to e.g. simulators. One usecase is
dumping the L2 at some point during simulation for some manipulations and then
reading it back again.

## Usage
### As a library
With your tclsh:
```bash
tclsh
source mem.tcl
```
Now first generate a verilog style dump from your elf:
```bash
riscv32-unknown-elf-objcopy -O verilog my-elf mem.verilog
```
or generate it from a PULP simulation:
```tclsh
pulp_dump_mem_l2 mem.verilog
```

Examples:

Change word width of a memory dump
```tclsh
# read into internal representation
set my_dump [read_mem_dump mem.verilog]

# change word width, also consider endianness
set my_dump change_width_mem_dump $my_dump 1 4 "little" "big"

# The addressing is still byte addressing, we need to change to word addressing
# The scaling of addresses is done relativ to the base_addr
set base_addr 0x1c000000
set my_dump [scale_addr_mem_dump $my_dump $base_addr 1 4]

# write back to a file
write_mem_dump $my_dump mem4.verilog
```

### As a script
You can use `mem.tcl` as a script to generate a memory image for each bank in L2
from an executable. This is mainly to bypass loading the executable via jtag
during simulation, which is very slow (512 kiB takes tens of minutes to load).

Usually you do something like this:
```bash
riscv32-unknown-elf-objcopy -O verilog my-elf preload.verilog
./mem.tcl preload.verilog out_dir
# when running the pulp/pulpissimo simulation pass the directory
vsim ... +preload=out_dir
```

You might think to use the `--verilog-data-width=val` option to generate a
different word width instead of using `mem.tcl` but this is broken for two
reasons:
1. the wrong endianness is used (always big-endian)
2. the wrong addressing is used (byte addressing instead of `val` width specific
   addressing). This conversion is anyway only possible by knowing the base
   address of memories.
