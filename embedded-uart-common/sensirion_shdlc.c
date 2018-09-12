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

#include "sensirion_arch_config.h"
#include "sensirion_uart.h"
#include "sensirion_shdlc.h"

#define SHDLC_START 0x7e
#define SHDLC_STOP 0x7e

#define SHDLC_MIN_TX_FRAME_SIZE 6
/** start/stop + (4 header + 255 data) * 2 because of byte stuffing */
#define SHDLC_FRAME_MAX_TX_FRAME_SIZE (2 + (4 + 255) * 2)

/** start/stop + (5 header + 255 data) * 2 because of byte stuffing */
#define SHDLC_FRAME_MAX_RX_FRAME_SIZE (2 + (5 + 255) * 2)

#define RX_DELAY_US 20000

static u8 sensirion_shdlc_crc(u8 header_sum, u8 data_len, const u8 *data) {
    header_sum += data_len;

    while (data_len--)
        header_sum += *(data++);

    return ~header_sum;
}

static u16 sensirion_shdlc_stuff_data(u8 data_len, const u8 *data,
                                      u8 *stuffed_data)
{
    u16 output_data_len = 0;
    u8 c;

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
            default:
                *(stuffed_data++) = c;
                output_data_len += 1;
        }
    }
    return output_data_len;
}

static u8 sensirion_shdlc_check_unstuff(u8 data) {
    return data == 0x7d;
}

static u8 sensirion_shdlc_unstuff_byte(u8 data) {
    switch (data) {
        case 0x31: return 0x11;
        case 0x33: return 0x13;
        case 0x5d: return 0x7d;
        case 0x5e: return 0x7e;
        default: return data;
    }
}

s16 sensirion_shdlc_xcv(u8 addr, u8 cmd, u8 tx_data_len, const u8 *tx_data,
                        u8 max_rx_data_len,
                        struct sensirion_shdlc_rx_header *rx_header,
                        u8 *rx_data) {
    s16 ret;

    ret = sensirion_shdlc_tx(addr, cmd, tx_data_len, tx_data);
    if (ret != 0)
        return ret;

    sensirion_sleep_usec(RX_DELAY_US);
    return sensirion_shdlc_rx(max_rx_data_len, rx_header, rx_data);
}

s16 sensirion_shdlc_tx(u8 addr, u8 cmd, u8 data_len, const u8 *data) {
    u16 len = 0;
    s16 ret;
    u8 crc;
    u8 tx_frame_buf[SHDLC_FRAME_MAX_TX_FRAME_SIZE];

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

s16 sensirion_shdlc_rx(u8 max_data_len, struct sensirion_shdlc_rx_header *rxh,
                       u8 *data) {
    u16 len;
    u16 i;
    u8 rx_frame[SHDLC_FRAME_MAX_RX_FRAME_SIZE];
    u8 *rx_header = (u8 *)rxh;
    u8 j;
    u8 crc;
    u8 unstuff_next;

    len = sensirion_uart_rx(2 + (5 + (u16)max_data_len) * 2, rx_frame);
    if (len < 1 || rx_frame[0] != SHDLC_START)
        return SENSIRION_SHDLC_ERR_MISSING_START;

    for (unstuff_next = 0, i = 1, j = 0;
         j < sizeof(*rxh) && i < len - 2;
         ++i) {
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

    for (unstuff_next = 0, j = 0;
         j < rxh->data_len && i < len - 2;
         ++i) {
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

    if (max_data_len < rxh->data_len) {
        rxh->data_len = max_data_len;
        return SENSIRION_SHDLC_ERR_MISSING_STOP;
    }

    if (j < rxh->data_len)
        return SENSIRION_SHDLC_ERR_ENCODING_ERROR;

    crc = rx_frame[i++];
    if (sensirion_shdlc_check_unstuff(crc)) {
        crc = sensirion_shdlc_unstuff_byte(rx_frame[++i]);
        if (i >= len)
            return SENSIRION_SHDLC_ERR_MISSING_STOP;
    }
    if (sensirion_shdlc_crc(rxh->addr + rxh->cmd + rxh->state, rxh->data_len,
                            data) != crc)
        return SENSIRION_SHDLC_ERR_CRC_MISMATCH;

    if (i >= len || rx_frame[i] != SHDLC_STOP)
        return SENSIRION_SHDLC_ERR_MISSING_STOP;

    return 0;
}
