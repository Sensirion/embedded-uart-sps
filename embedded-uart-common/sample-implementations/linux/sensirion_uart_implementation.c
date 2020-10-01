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
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "sensirion_arch_config.h"
#include "sensirion_uart.h"
#include <fcntl.h>
#include <stdio.h>
#include <termios.h>
#include <unistd.h>

// Adapted from
// http://www.raspberry-projects.com/pi/programming-in-c/uart-serial-port/using-the-uart

#ifndef SENSIRION_UART_TTYDEV
#define SENSIRION_UART_TTYDEV "/dev/ttyUSB0"
#endif

static int uart_fd = -1;

/**
 * sensirion_uart_select_port() - select the UART port index to use
 *                                THE IMPLEMENTATION IS OPTIONAL ON SINGLE-PORT
 *                                SETUPS (only one SPS30)
 *
 * Return:      0 on success, an error code otherwise
 */
int16_t sensirion_uart_select_port(uint8_t port) {
    return 0;
}

int16_t sensirion_uart_open() {
    // The flags (defined in fcntl.h):
    //    Access modes (use 1 of these):
    //        O_RDONLY - Open for reading only.
    //        O_RDWR - Open for reading and writing.
    //        O_WRONLY - Open for writing only.
    //    O_NDELAY / O_NONBLOCK (same function) - Enables nonblocking mode.
    //      When set read requests on the file can return immediately with a
    //      failure status if there is no input immediately available (instead
    //      of blocking). Likewise, write requests can also return immediately
    //      with a failure status if the output can't be written immediately.
    //    O_NOCTTY - When set and path identifies a terminal device, open()
    //      shall not cause the terminal device to become the controlling
    //      terminal for the process.
    uart_fd = open(SENSIRION_UART_TTYDEV, O_RDWR | O_NOCTTY);
    if (uart_fd == -1) {
        fprintf(stderr, "Error opening UART. Ensure it's not otherwise used\n");
        return -1;
    }

    // see http://pubs.opengroup.org/onlinepubs/007908799/xsh/termios.h.html:
    //    CSIZE:- CS5, CS6, CS7, CS8
    //    CLOCAL - Ignore modem status lines
    //    CREAD - Enable receiver
    //    IGNPAR = Ignore characters with parity errors
    //    ICRNL - Map CR to NL on input (Use for ASCII comms where you want to
    //                                   auto correct end of line characters,
    //                                   don't use for bianry comms)
    //    PARENB - Parity enable
    //    PARODD - Odd parity (else even)
    struct termios options;
    tcgetattr(uart_fd, &options);
    options.c_cflag = B115200 | CS8 | CLOCAL | CREAD;  // set baud rate
    options.c_iflag = IGNPAR;
    options.c_oflag = 0;
    options.c_lflag = 0;
    tcflush(uart_fd, TCIFLUSH);
    tcsetattr(uart_fd, TCSANOW, &options);
    return 0;
}

int16_t sensirion_uart_close() {
    return close(uart_fd);
}

int16_t sensirion_uart_tx(uint16_t data_len, const uint8_t* data) {
    if (uart_fd == -1)
        return -1;

    return write(uart_fd, (void*)data, data_len);
}

int16_t sensirion_uart_rx(uint16_t max_data_len, uint8_t* data) {
    if (uart_fd == -1)
        return -1;

    return read(uart_fd, (void*)data, max_data_len);
}
void sensirion_sleep_usec(uint32_t useconds) {
    usleep(useconds);
}
