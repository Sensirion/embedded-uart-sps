/*
 * Copyright (c) 2018, Sensirion AG
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * * Neither the name of Sensirion AG nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef SENSIRION_UART_H
#define SENSIRION_UART_H

#ifdef __cplusplus
extern "C" {
#endif

#include "sensirion_arch_config.h"

/**
 * sensirion_uart_open() - initialize UART
 *
 * Return:      0 on success, an error code otherwise
 */
s16 sensirion_uart_open();

/**
 * sensirion_uart_close() - release UART resources
 *
 * Return:      0 on success, an error code otherwise
 */
s16 sensirion_uart_close();

/**
 * sensirion_uart_tx() - transmit data over UART
 *
 * @data_len:   number of bytes to send
 * @data:       data to send
 * Return:      Number of bytes sent or a negative error code
 */
s16 sensirion_uart_tx(u16 data_len, const u8 *data);

/**
 * sensirion_uart_rx() - receive data over UART
 *
 * @data_len:   max number of bytes to receive
 * @data:       Memory where received data is stored
 * Return:      Number of bytes received or a negative error code
 */
s16 sensirion_uart_rx(u16 max_data_len, u8 *data);

/**
 * Sleep for a given number of microseconds. The function should delay the
 * execution for at least the given time, but may also sleep longer.
 *
 * Despite the unit, a <10 millisecond precision is sufficient.
 *
 * @param useconds the sleep time in microseconds
 */
void sensirion_sleep_usec(u32 useconds);

#ifdef __cplusplus
}
#endif

#endif /* SENSIRION_UART_H */
