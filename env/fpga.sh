# FPGA specific compilation options. Needs to be sourced last.

# we don't have fake io on FPGAs
export CONFIG_STDIO=uart
