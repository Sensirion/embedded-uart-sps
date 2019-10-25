sps_driver_dir ?= ..
sensirion_common_dir ?= ${sps_driver_dir}/embedded-uart-common
sps_common_dir ?= ${sps_driver_dir}/sps-common
sps30_uart_dir ?= ${sps_driver_dir}/sps30-uart

uart_sources ?= ${sensirion_common_dir}/sensirion_uart_implementation.c

CFLAGS ?= -Os -Wall -fstrict-aliasing -Wstrict-aliasing=1 -Wsign-conversion -fPIC
CFLAGS += -I${sensirion_common_dir} -I${sps_common_dir} -I${sps30_uart_dir} \

sensirion_common_sources = ${sensirion_common_dir}/sensirion_arch_config.h \
                           ${sensirion_common_dir}/sensirion_uart.h \
                           ${sensirion_common_dir}/sensirion_shdlc.h \
                           ${sensirion_common_dir}/sensirion_shdlc.c

sps_common_sources = ${sps_common_dir}/sps_git_version.h \
                     ${sps_common_dir}/sps_git_version.c

sps30_uart_sources = ${sensirion_common_sources} ${sps_common_sources} \
                     ${sps30_uart_dir}/sps30.h ${sps30_uart_dir}/sps30.c
