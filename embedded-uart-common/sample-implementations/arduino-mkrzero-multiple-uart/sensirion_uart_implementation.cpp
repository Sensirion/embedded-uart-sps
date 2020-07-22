/*
 * Copyright (c) 2019, Sensirion AG
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

// needed for delay() routine
#include "wiring_private.h"  // pinPeripheral() function
#include <Arduino.h>

#include "sensirion_uart.h"

#define BAUDRATE 115200  // baud rate of SPS30
#define PIN_UART_2_RX 7
#define PIN_UART_2_TX 6
#define PIN_UART_3_RX 29
#define PIN_UART_3_TX 28

Uart Serial2(&sercom3, 7, 6, PAD_SERIAL1_RX, PAD_SERIAL1_TX);
Uart Serial3(&sercom4, 29, 28, PAD_SERIAL1_RX, PAD_SERIAL1_TX);

void SERCOM3_Handler() {
    Serial2.IrqHandler();
}

void SERCOM4_Handler() {
    Serial3.IrqHandler();
}

#ifdef __cplusplus
extern "C" {
#endif

static Uart* const ports[] = {&Serial2, &Serial3};
static uint8_t cur_port = 0;

/**
 * sensirion_uart_select_port() - select the UART port index to use
 *                                THE IMPLEMENTATION IS OPTIONAL ON SINGLE-PORT
 *                                SETUPS (only one SPS30)
 *
 * Return:      0 on success, -1 if the selected port is invalid
 */
int16_t sensirion_uart_select_port(uint8_t port) {
    if (port >= (sizeof(ports) / sizeof(ports[0])))
        return -1;
    cur_port = port;
    return 0;
}

/**
 * sensirion_uart_open() - initialize UART
 *
 * Return:      0 on success, -1 if the selected port is invalid
 */
int16_t sensirion_uart_open() {
    switch (cur_port) {
        case 0:
            Serial2.begin(BAUDRATE);
            pinPeripheral(PIN_UART_2_RX, PIO_SERCOM_ALT);
            pinPeripheral(PIN_UART_2_TX, PIO_SERCOM_ALT);
            return 0;

        case 1:
            Serial3.begin(BAUDRATE);
            pinPeripheral(PIN_UART_3_RX, PIO_SERCOM_ALT);
            pinPeripheral(PIN_UART_3_TX, PIO_SERCOM_ALT);
            return 0;

        default:
            return -1;
    }
}

/**
 * sensirion_uart_close() - release UART resources
 *
 * Return:      0 on success, -1 if the selected port is invalid
 */
int16_t sensirion_uart_close() {
    switch (cur_port) {
        case 0:
            Serial2.end();
            return 0;

        case 1:
            Serial3.end();
            return 0;

        default:
            return -1;
    }
}

/**
 * sensirion_uart_tx() - transmit data over UART
 *
 * @data_len:   number of bytes to send
 * @data:       data to sendv v
 * Return:      Number of bytes sent or a negative error code
 */
int16_t sensirion_uart_tx(uint16_t data_len, const uint8_t* data) {
    return ports[cur_port]->write(data, data_len);
}

/**
 * sensirion_uart_rx() - receive data over UART
 *
 * @data_len:   max number of bytes to receive
 * @data:       Memory where received data is stored
 * Return:      Number of bytes received or a negative error code
 */
int16_t sensirion_uart_rx(uint16_t max_data_len, uint8_t* data) {
    int16_t i;

    for (i = 0; ports[cur_port]->available() > 0 && i < max_data_len; ++i)
        data[i] = (uint8_t)(ports[cur_port]->read());

    return i;
}

/**
 * Sleep for a given number of microseconds. The function should delay the
 * execution for at least the given time, but may also sleep longer.
 *
 * Despite the unit, a <10 millisecond precision is sufficient.
 *
 * @param useconds the sleep time in microseconds
 */
void sensirion_sleep_usec(uint32_t useconds) {
    delay((useconds / 1000) + 1);
}

#ifdef __cplusplus
}  // extern "C"
#endif
