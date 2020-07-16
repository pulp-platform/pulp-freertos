# PULP PROJECT FREERTOS [![pipeline status](https://iis-git.ee.ethz.ch/freertos/freertos/badges/master/pipeline.svg)](https://iis-git.ee.ethz.ch/freertos/freertos/commits/master)

This project provides FreeRTOS and drivers for development of real-time
applications on PULP based systems.

Programs can be run using RTL simulation (simulating the hardware design) or the
virtual platform (called gvsoc, software emulation of the hardware design).

## Getting Started
You need the following items to develop and run FreeRTOS programs for PULP
* A centos7 or ubuntu18 based workstation
* This repository
* A C compiler that supports the RISC-V instruction set, either pulp-riscv-gcc
  or riscv-gcc.
* (optional) A compiled PULP hardware design if you intend to use rtl simulation

At the end of this section you should be able to run your own hello world
program, either using the virtual platform or a rtl simulation of a PULP based
platform.

### Repository
Run
```bash
git clone git@iis-git.ee.ethz.ch:freertos/freertos.git --recurse-submodules
```
in your shell. The `--recurse-submodules` argument is mandatory.

### Compiler
If you want to use the PULP extension (additional instructions such as hardware
loops, DSP etc.) then get the source code from
[here](https://github.com/pulp-platform/pulp-riscv-gnu-toolchain) and compile it
according to the manual there or (easier) download the latest binary release
from [here](https://github.com/pulp-platform/pulp-riscv-gnu-toolchain/releases).
Make sure you choose the correct distribution, otherwise you might run into ABI
issues.

If you want to use upstream RISC-V gcc then you can get it from
[here](https://github.com/riscv/riscv-gnu-toolchain), but you need to configure
and compile it yourself.

Now make sure that either
* the installation path of the newly acquired compiler is in your `PATH` environment variable or
* you set the `RISCV` environment variable to point to it

In both cases you need to point to the "root" folder of the compiler
installation, meaning you point to the directory that contains `bin`, `include`,
`lib` etc.

### (Optional) PULP RTL design
Get the latest release of either [PULP](https://github.com/pulp-platform/pulp)
or [PULPissimo](https://github.com/pulp-platform/pulpissimo/) and compile it by
running `make build`. Afterwards make sure you source the configuration files to
make this repository aware of your compiled RTL platform by calling `source
setup/vsim.sh`

### Setting up and running a hello world
Now that we have a compiler and a platform to run our programs on, we can set up
a simple hello world, a program that prints `"hello world"` to `stdout`. Note
that `stdout` is both in the rtl simulation and the virtual platform represented
by a pseudo device (i.e. not existing in real-hardware). This is useful for
debugging purposes, but for a more realistic scenario you can look a the
`test/uart` program.

1. Copy the `template/helloworld` into a directory of your chosing
2. Run `source env/your-platform.sh` to target the desired platform and make
   `helloworld` aware where to find the FreeRTOS source files and drivers
3. Run `make all` to compile the example program
4. Call `make run-sim` to do an RTL simulation or `make run-gvsoc` to do a
   virtual platform simulation of the example program

You should see "hello world" being printed to your terminal.

### Setting up and running blinky
Blinky is the canonical FreeRTOS example program. On hardware (FPGA) it will
blink a led, in the RTL simulation it will toggle an gpio pad and print "blink"
to stdout and on the virtual platform we just get the latter.

(TODO)

#### Tree of your (typical) project
Once you have compiled a program your project's tree will roughly look like this
(here demonstranted on helloworld).

```
.
├── FreeRTOSConfig.h     FreeRTOS specific macros that enable/disable features
├── gvsim                Simulation output of a virtual platform simulation
├── helloworld           Your compiled program
├── helloworld.c         Source files
├── helloworld.hex       Hexdump of your program (not used)
├── helloworld.lst       Disassembly, symbol table and header of your program
├── helloworld.map       Linker map of your program (linked and discarded sections)
├── helloworld.o         Object files
├── helloworld.stim      Stimuli files (format that can be used by rtl simulation)
├── helloworld.veri      Verilog memory dump for faster rtl simulations
├── Makefile             Compile, run, analyze. Check make target
└── sim                  Simulation output of a RTL simulation
```


### TL;DR
1. Install compiler, point `RISCV` and/or the `PATH` to it
2. Use one of the given templates and copy it
3. (Optional) Install the RTL platform with `make build` and refer to it with `source
   setup/vsim.sh`
4. `source env/your-platform.sh` in this project's root to make the template
   aware of this project source files
5. `make all run-sim` or `make all run-gvsoc` to compile and simulate

## Directory structure
```
.
├── apps               Applications that use this FreeRTOS port
├── bench              RTOS benchmarks
├── common             Driver code and build system
├── CONTRIBUTING.md    How to contribute to this repository
├── demos              Classic FreeRTOS blinky demo
├── env                Sourcable configuration files to target the desired platform
├── kernel             FreeRTOs kernel code with PULP specific patches
├── nortos             Simple programs that don't need FreeRTOS
├── README.md          Read this
├── scripts            Various analysis scripts
├── support            Support libraries for simulation (gvsoc, dpi, etc.)
├── template           Template projects to get started
└── tests              Advanced tests using FreeRTOS primitives
```

## Developer Notes
`assert()` from `#include <assert.h>` calls `__assert_func`, `fiprintf` then
`abort`. This is all in newlib.
