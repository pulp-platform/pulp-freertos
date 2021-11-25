# default feature settings for compiling programs

# generic
export CONFIG_FREERTOS_KERNEL=y
export CONFIG_FREERTOS_CHIP_INCLUDE=PULPissimo
export CONFIG_USE_NEWLIB=y

# stdio
export CONFIG_STDIO=fake
## uart id to be used for stdio (if enabled)
export CONFIG_STDIO_UART_DEVICE_ID=0
## uart baudrate (if enabled)
export CONFIG_STDIO_UART_BAUDRATE=115200
## uart internal copy out buffer size (if enabled)
export CONFIG_STDIO_UART_BUFSIZE=256

# divers
export CONFIG_DRIVER_FLL=y
export CONFIG_DRIVER_CLKDIV=n
export CONFIG_DRIVER_PLIC=n
export CONFIG_DRIVER_INT=pclint

# compilation options
export CONFIG_CC_LTO=n
export CONFIG_CC_SANITIZE=n
export CONFIG_CC_STACKDBG=n
export CONFIG_CC_MAXSTACKSIZE=1024
