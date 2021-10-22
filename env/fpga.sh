# Generic FPGA specific compilation options. Needs to be sourced last.

# we don't have fake io on FPGAs
export CONFIG_STDIO=uart
# constant clock sources
export CONFIG_DRIVER_FLL=n
export CONFIG_DRIVER_CLKDIV=n
export CONFIG_DRIVER_CLKCONST=y
