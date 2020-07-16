
# PULP PROJECT FREERTOS [![pipeline status](https://iis-git.ee.ethz.ch/freertos/freertos/badges/pulp-project-devel-10.3.1/pipeline.svg)](https://iis-git.ee.ethz.ch/freertos/freertos/commits/pulp-project-devel-10.3.1)

## Quick start (running a simulation)
1. Set up `VSIM_PATH` correctly by sourcing `setup/vsim.sh` in your PULP project.
Make sure it is compiled.
2. Enter program directory and call `make all run` to start simulating.

## Creating your own firmware based on FreeRTOS
Have a look at `demos/main.c` and `demos/Makefile`

## Notes
`assert()` from `#include <assert.h>` calls `__assert_func`, `fiprintf` then
`abort`. This is all in newlib.
