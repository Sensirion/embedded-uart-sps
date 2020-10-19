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

#include "sensirion_shdlc.h"
#include "sensirion_arch_config.h"
#include "sensirion_uart.h"

#define SHDLC_START 0x7e
#define SHDLC_STOP 0x7e

#define SHDLC_MIN_TX_FRAME_SIZE 6
/** start/stop + (4 header + 255 data) * 2 because of byte stuffing */
#define SHDLC_FRAME_MAX_TX_FRAME_SIZE (2 + (4 + 255) * 2)

/** start/stop + (5 header + 255 data) * 2 because of byte stuffing */
#define SHDLC_FRAME_MAX_RX_FRAME_SIZE (2 + (5 + 255) * 2)

#define RX_DELAY_US 20000

uint16_t sensirion_bytes_to_uint16_t(const uint8_t* bytes) {
    return (uint16_t)bytes[0] << 8 | (uint16_t)bytes[1];
}

uint32_t sensirion_bytes_to_uint32_t(const uint8_t* bytes) {
    return (uint32_t)bytes[0] << 24 | (uint32_t)bytes[1] << 16 |
           (uint32_t)bytes[2] << 8 | (uint32_t)bytes[3];
}

int16_t sensirion_bytes_to_int16_t(const uint8_t* bytes) {
    return (int16_t)sensirion_bytes_to_uint16_t(bytes);
}

int32_t sensirion_bytes_to_int32_t(const uint8_t* bytes) {
    return (int32_t)sensirion_bytes_to_uint32_t(bytes);
}

float sensirion_bytes_to_float(const uint8_t* bytes) {
    union {
        uint32_t u32_value;
        float float32;
    } tmp;

    tmp.u32_value = sensirion_bytes_to_uint32_t(bytes);
    return tmp.float32;
}

void sensirion_uint32_t_to_bytes(const uint32_t value, uint8_t* bytes) {
    bytes[0] = value >> 24;
    bytes[1] = value >> 16;
    bytes[2] = value >> 8;
    bytes[3] = value;
}

void sensirion_uint16_t_to_bytes(const uint16_t value, uint8_t* bytes) {
    bytes[0] = value >> 8;
    bytes[1] = value;
}

void sensirion_int32_t_to_bytes(const int32_t value, uint8_t* bytes) {
    bytes[0] = value >> 24;
    bytes[1] = value >> 16;
    bytes[2] = value >> 8;
    bytes[3] = value;
}

void sensirion_int16_t_to_bytes(const int16_t value, uint8_t* bytes) {
    bytes[0] = value >> 8;
    bytes[1] = value;
}

void sensirion_float_to_bytes(const float value, uint8_t* bytes) {
    union {
        uint32_t u32_value;
        float float32;
    } tmp;
    tmp.float32 = value;
    sensirion_uint32_t_to_bytes(tmp.u32_value, bytes);
}

static uint8_t sensirion_shdlc_crc(uint8_t header_sum, uint8_t data_len,
                                   const uint8_t* data) {
    header_sum += data_len;

    while (data_len--)
        header_sum += *(data++);

    return ~header_sum;
}

static uint16_t sensirion_shdlc_stuff_data(uint8_t data_len,
                                           const uint8_t* data,
                                           uint8_t* stuffed_data) {
    uint16_t output_data_len = 0;
    uint8_t c;

    while (data_len--) {
        c = *(data++);
        switch (c) {
            case 0x11:
            case 0x13:
            case 0x7d:
            case 0x7e:
                // byte stuffing is done by inserting 0x7d and inverting bit 5
                *(stuffed_data++) = 0x7d;
                *(stuffed_data++) = c ^ (1 << 5);
                output_data_len += 2;
                break;
            default:
                *(stuffed_data++) = c;
                output_data_len += 1;
        }
    }
    return output_data_len;
}

static uint8_t sensirion_shdlc_check_unstuff(uint8_t data) {
    return data == 0x7d;
}

static uint8_t sensirion_shdlc_unstuff_byte(uint8_t data) {
    switch (data) {
        case 0x31:
            return 0x11;
        case 0x33:
            return 0x13;
        case 0x5d:
            return 0x7d;
        case 0x5e:
            return 0x7e;
        default:
            return data;
    }
}

int16_t sensirion_shdlc_xcv(uint8_t addr, uint8_t cmd, uint8_t tx_data_len,
                            const uint8_t* tx_data, uint8_t max_rx_data_len,
                            struct sensirion_shdlc_rx_header* rx_header,
                            uint8_t* rx_data) {
    int16_t ret;

    ret = sensirion_shdlc_tx(addr, cmd, tx_data_len, tx_data);
    if (ret != 0)
        return ret;

    sensirion_sleep_usec(RX_DELAY_US);
    return sensirion_shdlc_rx(max_rx_data_len, rx_header, rx_data);
}

int16_t sensirion_shdlc_tx(uint8_t addr, uint8_t cmd, uint8_t data_len,
                           const uint8_t* data) {
    uint16_t len = 0;
    int16_t ret;
    uint8_t crc;
    uint8_t tx_frame_buf[SHDLC_FRAME_MAX_TX_FRAME_SIZE];

    crc = sensirion_shdlc_crc(addr + cmd, data_len, data);

    tx_frame_buf[len++] = SHDLC_START;
    len += sensirion_shdlc_stuff_data(1, &addr, tx_frame_buf + len);
    len += sensirion_shdlc_stuff_data(1, &cmd, tx_frame_buf + len);
    len += sensirion_shdlc_stuff_data(1, &data_len, tx_frame_buf + len);
    len += sensirion_shdlc_stuff_data(data_len, data, tx_frame_buf + len);
    len += sensirion_shdlc_stuff_data(1, &crc, tx_frame_buf + len);
    tx_frame_buf[len++] = SHDLC_STOP;

    ret = sensirion_uart_tx(len, tx_frame_buf);
    if (ret < 0)
        return ret;
    if (ret != len)
        return SENSIRION_SHDLC_ERR_TX_INCOMPLETE;
    return 0;
}

int16_t sensirion_shdlc_rx(uint8_t max_data_len,
                           struct sensirion_shdlc_rx_header* rxh,
                           uint8_t* data) {
    int16_t len;
    uint16_t i;
    uint8_t rx_frame[SHDLC_FRAME_MAX_RX_FRAME_SIZE];
    uint8_t* rx_header = (uint8_t*)rxh;
    uint8_t j;
    uint8_t crc;
    uint8_t unstuff_next;

    len = sensirion_uart_rx(2 + (5 + (uint16_t)max_data_len) * 2, rx_frame);
    if (len < 1 || rx_frame[0] != SHDLC_START)
        return SENSIRION_SHDLC_ERR_MISSING_START;

    for (unstuff_next = 0, i = 1, j = 0; j < sizeof(*rxh) && i < len - 2; ++i) {
        if (unstuff_next) {
            rx_header[j++] = sensirion_shdlc_unstuff_byte(rx_frame[i]);
            unstuff_next = 0;
        } else {
            unstuff_next = sensirion_shdlc_check_unstuff(rx_frame[i]);
            if (!unstuff_next)
                rx_header[j++] = rx_frame[i];
        }
    }
    if (j != sizeof(*rxh) || unstuff_next)
        return SENSIRION_SHDLC_ERR_ENCODING_ERROR;

    if (max_data_len < rxh->data_len)
        return SENSIRION_SHDLC_ERR_FRAME_TOO_LONG; /* more data than expected */

    for (unstuff_next = 0, j = 0; j < rxh->data_len && i < len - 2; ++i) {
        if (unstuff_next) {
            data[j++] = sensirion_shdlc_unstuff_byte(rx_frame[i]);
            unstuff_next = 0;
        } else {
            unstuff_next = sensirion_shdlc_check_unstuff(rx_frame[i]);
            if (!unstuff_next)
                data[j++] = rx_frame[i];
        }
    }

    if (unstuff_next)
        return SENSIRION_SHDLC_ERR_ENCODING_ERROR;

    if (j < rxh->data_len)
        return SENSIRION_SHDLC_ERR_ENCODING_ERROR;

    crc = rx_frame[i++];
    if (sensirion_shdlc_check_unstuff(crc))
        crc = sensirion_shdlc_unstuff_byte(rx_frame[i++]);

    if (sensirion_shdlc_crc(rxh->addr + rxh->cmd + rxh->state, rxh->data_len,
                            data) != crc)
        return SENSIRION_SHDLC_ERR_CRC_MISMATCH;

    if (i >= len || rx_frame[i] != SHDLC_STOP)
        return SENSIRION_SHDLC_ERR_MISSING_STOP;

    return 0;
}
