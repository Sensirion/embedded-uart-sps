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

// needed for delay() routine
#include "wiring_private.h"  // pinPeripheral() function
#include <Arduino.h>

#ifdef __cplusplus
extern "C" {
#endif

#include "sensirion_arch_config.h"
#include "sensirion_uart.h"

#define BAUDRATE 115200  // baud rate of SPS30
#define PIN_UART_RX 11
#define PIN_UART_TX 10

/*
 * Create a new serial interface on pin 10 (TX) and 11 (RX)
 * Source:
 * https://learn.adafruit.com/using-atsamd21-sercom-to-add-more-spi-i2c-serial-ports/creating-a-new-serial
 *
 * NOTE: The procedure is different for non-SAMD based boards like the Arduino
 *       Uno where `Serial` is usually shared with the USB connection, thus an
 *       alternative port may have to be established, e.g. with SoftwareSerial,
 *       see
 * http://www.fiz-ix.com/2012/12/arduino-uno-with-multiple-software-serial-devices/
 */
Uart Serial2(&sercom1, PIN_UART_RX, PIN_UART_TX, SERCOM_RX_PAD_0,
             UART_TX_PAD_2);

void SERCOM1_Handler() {
    Serial2.IrqHandler();
}

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

/**
 * sensirion_uart_open() - initialize UART
 *
 * Return:      0 on success, an error code otherwise
 */
int16_t sensirion_uart_open() {
    Serial2.begin(BAUDRATE);
    pinPeripheral(PIN_UART_TX, PIO_SERCOM);
    pinPeripheral(PIN_UART_RX, PIO_SERCOM);

    while (!Serial) {
        delay(100);
    }
    return 0;
}

/**
 * sensirion_uart_close() - release UART resources
 *
 * Return:      0 on success, an error code otherwise
 */
int16_t sensirion_uart_close() {
    Serial2.end();
    return 0;
}

/**
 * sensirion_uart_tx() - transmit data over UART
 *
 * @data_len:   number of bytes to send
 * @data:       data to sendv v
 * Return:      Number of bytes sent or a negative error code
 */
int16_t sensirion_uart_tx(uint16_t data_len, const uint8_t* data) {
    return Serial2.write(data, data_len);
}

/**
 * sensirion_uart_rx() - receive data over UART
 *
 * @data_len:   max number of bytes to receive
 * @data:       Memory where received data is stored
 * Return:      Number of bytes received or a negative error code
 */
int16_t sensirion_uart_rx(uint16_t max_data_len, uint8_t* data) {
    int16_t i = 0;

    while (Serial2.available() > 0 && i < max_data_len) {
        data[i] = (uint8_t)Serial2.read();
        i++;
    }

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
