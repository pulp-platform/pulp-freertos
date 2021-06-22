# default feature settings for compiling programs

# generic
export CONFIG_FREERTOS_KERNEL=y
export CONFIG_USE_NEWLIB=y

# divers
export CONFIG_DRIVER_FLL=y
export CONFIG_DRIVER_CLKDIV=n
export CONFIG_DRIVER_PLIC=n

# compilation options
export CONFIG_CC_LTO=n
export CONFIG_CC_SANITIZE=n
export CONFIG_CC_STACKDBG=n
export CONFIG_CC_MAXSTACKSIZE=1024
