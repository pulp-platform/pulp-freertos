
# PULP PROJECT FREERTOS [![pipeline status](https://iis-git.ee.ethz.ch/freertos/freertos/badges/master/pipeline.svg)](https://iis-git.ee.ethz.ch/freertos/freertos/commits/master)

## Quick start (running a simulation)
1. Set up `VSIM_PATH` correctly by sourcing `setup/vsim.sh` in your PULP project.
Make sure it is compiled.
2. Call `make all run` to start simulating FreeRTOS' blinky.


## Notes
`assert()` from `#include <assert.h>` calls `__assert_func`, `fiprintf` then
`abort`. This is all in newlib.
