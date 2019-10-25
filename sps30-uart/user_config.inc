## This file controls the custom user build settings.

## For UART, configure the UART HAL implementation to use.
## Use one of the available sample-implementations or implement your own using
## the stub.
# uart_sources = ${sensirion_common_dir}/sensirion_uart_implementation.c

##
## The items below are listed as documentation but may not need customization
##

## The build paths can also be changed here if needed
# sps_driver_dir = ..
# sensirion_common_dir = ${sps_driver_dir}/embedded-uart-common
# sps_common_dir = ${sps_driver_dir}/sps-common
# sps30_uart_dir = ${sps_driver_dir}/sps30-uart

## If you need different CFLAGS, those can be customized as well
# CFLAGS = -Os -Wall -fstrict-aliasing -Wstrict-aliasing=1 -Wsign-conversion -fPIC
