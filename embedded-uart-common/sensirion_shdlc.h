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

#ifndef SENSIRION_SHDLC_H
#define SENSIRION_SHDLC_H

#ifdef __cplusplus
extern "C" {
#endif

#include "sensirion_arch_config.h"

#define SENSIRION_SHDLC_ERR_NO_DATA -1
#define SENSIRION_SHDLC_ERR_MISSING_START -2
#define SENSIRION_SHDLC_ERR_MISSING_STOP -3
#define SENSIRION_SHDLC_ERR_CRC_MISMATCH -4
#define SENSIRION_SHDLC_ERR_ENCODING_ERROR -5
#define SENSIRION_SHDLC_ERR_TX_INCOMPLETE -6
#define SENSIRION_SHDLC_ERR_FRAME_TOO_LONG -7

#if SENSIRION_BIG_ENDIAN
#define be16_to_cpu(s) (s)
#define be32_to_cpu(s) (s)
#define be64_to_cpu(s) (s)
#else
#define be16_to_cpu(s) (((uint16_t)(s) << 8) | (0xff & ((uint16_t)(s)) >> 8))
#define be32_to_cpu(s)                                                         \
    (((uint32_t)be16_to_cpu(s) << 16) | (0xffff & (be16_to_cpu((s) >> 16))))
#define be64_to_cpu(s)                                                         \
    (((uint64_t)be32_to_cpu(s) << 32) |                                        \
     (0xffffffff & ((uint64_t)be32_to_cpu((s) >> 32))))
#endif

struct sensirion_shdlc_rx_header {
    uint8_t addr;
    uint8_t cmd;
    uint8_t state;
    uint8_t data_len;
};

/**
 * sensirion_shdlc_tx() - transmit an SHDLC frame
 *
 * @addr:       SHDLC recipient address
 * @cmd:        command parameter
 * @data_len:   data length to send
 * @data:       data to send
 * Return:      0 on success, an error code otherwise
 */
int16_t sensirion_shdlc_tx(uint8_t addr, uint8_t cmd, uint8_t data_len,
                           const uint8_t *data);

/**
 * sensirion_shdlc_rx() - receive an SHDLC frame
 *
 * Note that the header and data must be discarded on failure
 *
 * @data_len:   max data length to receive
 * @header:     Memory where the SHDLC header containing the sender address,
 *              command, sensor state and data length is stored
 * @data:       Memory where received data is stored
 * Return:      0 on success, an error code otherwise
 */
int16_t sensirion_shdlc_rx(uint8_t max_data_len,
                           struct sensirion_shdlc_rx_header *header,
                           uint8_t *data);

/**
 * sensirion_shdlc_xcv() - transceive (transmit then receive) an SHDLC frame
 *
 * Note that rx_header and rx_data must be discarded on failure
 *
 * @addr:           recipient address
 * @cmd:            parameter
 * @tx_data_len:    data length to send
 * @tx_data:        data to send
 * @rx_header:      Memory where the SHDLC header containing the sender address,
 *                  command, sensor state and data length is stored
 * @rx_data:        Memory where the received data is stored
 * Return:          0 on success, an error code otherwise
 */
int16_t sensirion_shdlc_xcv(uint8_t addr, uint8_t cmd, uint8_t tx_data_len,
                            const uint8_t *tx_data, uint8_t max_rx_data_len,
                            struct sensirion_shdlc_rx_header *rx_header,
                            uint8_t *rx_data);

#ifdef __cplusplus
}
#endif

#endif /* SENSIRION_SHDLC_H */
