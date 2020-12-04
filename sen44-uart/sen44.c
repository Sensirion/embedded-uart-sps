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

#include "sen44.h"
#include "sensirion_shdlc.h"
#include "sps_git_version.h"

#define SEN44_ADDR 0x00
#define SEN44_CMD_START_MEASUREMENT 0x00
#define SEN44_CMD_STOP_MEASUREMENT 0x01
#define SEN44_MEASUREMENT_MODE \
    { 0x02 }
#define SEN44_CMD_READ_MEASUREMENT 0x03
#define SEN44_SUBCMD_READ_MEASUREMENT \
    { 0x07 }
#define SEN44_CMD_DEV_INFO 0xd0
#define SEN44_CMD_DEV_INFO_SUBCMD_GET_SERIAL \
    { 0x03 }
#define SEN44_CMD_READ_VERSION 0xd1
#define SEN44_CMD_READ_DEV_STATUS_REG 0xd2
#define SEN44_CMD_RESET 0xd3
#define SEN44_ERR_STATE(state) (SEN44_ERR_STATE_MASK | (state))

const char* sen44_get_driver_version(void) {
    return SPS_DRV_VERSION_STR;
}

int16_t sen44_probe(void) {
    char serial[SEN44_MAX_SERIAL_LEN];
    int16_t error = sen44_get_serial(serial);

    return error;
}

int16_t sen44_get_serial(char* serial) {
    struct sensirion_shdlc_rx_header header;
    uint8_t param_buf[] = SEN44_CMD_DEV_INFO_SUBCMD_GET_SERIAL;
    int16_t error;

    error = sensirion_shdlc_xcv(
        SEN44_ADDR, SEN44_CMD_DEV_INFO, sizeof(param_buf), param_buf,
        SEN44_MAX_SERIAL_LEN, &header, (uint8_t*)serial);
    if (error < 0) {
        return error;
    }

    if (header.state) {
        return SEN44_ERR_STATE(header.state);
    }

    return 0;
}

int16_t sen44_start_measurement(void) {
    struct sensirion_shdlc_rx_header header;
    uint8_t param_buf[] = SEN44_MEASUREMENT_MODE;

    return sensirion_shdlc_xcv(SEN44_ADDR, SEN44_CMD_START_MEASUREMENT,
                               sizeof(param_buf), param_buf, 0, &header,
                               (uint8_t*)NULL);
}

int16_t sen44_stop_measurement(void) {
    struct sensirion_shdlc_rx_header header;

    return sensirion_shdlc_xcv(SEN44_ADDR, SEN44_CMD_STOP_MEASUREMENT, 0,
                               (uint8_t*)NULL, 0, &header, (uint8_t*)NULL);
}

int16_t sen44_read_measurement(struct sen44_measurement* measurement) {
    struct sensirion_shdlc_rx_header header;
    uint8_t param_buf[] = SEN44_SUBCMD_READ_MEASUREMENT;
    int16_t error;
    uint16_t idx;
    uint16_t data[sizeof(struct sen44_measurement) / sizeof(int16_t)];

    error = sensirion_shdlc_xcv(SEN44_ADDR, SEN44_CMD_READ_MEASUREMENT,
                                sizeof(param_buf), param_buf, sizeof(data),
                                &header, (uint8_t*)data);
    if (error) {
        return error;
    }

    if (header.data_len != sizeof(data)) {
        return SEN44_ERR_NOT_ENOUGH_DATA;
    }

    idx = 0;
    measurement->mc_1p0 = sensirion_bytes_to_uint16_t((uint8_t*)&data[idx]);
    ++idx;
    measurement->mc_2p5 = sensirion_bytes_to_uint16_t((uint8_t*)&data[idx]);
    ++idx;
    measurement->mc_4p0 = sensirion_bytes_to_uint16_t((uint8_t*)&data[idx]);
    ++idx;
    measurement->mc_10p0 = sensirion_bytes_to_uint16_t((uint8_t*)&data[idx]);
    ++idx;
    measurement->voc_index =
        (int16_t)sensirion_bytes_to_uint16_t((uint8_t*)&data[idx]);
    ++idx;
    measurement->ambient_humidity =
        (int16_t)sensirion_bytes_to_uint16_t((uint8_t*)&data[idx]);
    ++idx;
    measurement->ambient_temperature =
        (int16_t)sensirion_bytes_to_uint16_t((uint8_t*)&data[idx]);

    if (header.state) {
        return SEN44_ERR_STATE(header.state);
    }

    return 0;
}

int16_t
sen44_read_version(struct sen44_version_information* version_information) {
    struct sensirion_shdlc_rx_header header;
    int16_t error;
    uint8_t data[7];

    error = sensirion_shdlc_xcv(SEN44_ADDR, SEN44_CMD_READ_VERSION, 0,
                                (uint8_t*)NULL, sizeof(data), &header, data);
    if (error) {
        return error;
    }

    if (header.data_len != sizeof(data)) {
        return SEN44_ERR_NOT_ENOUGH_DATA;
    }

    if (header.state) {
        return SEN44_ERR_STATE(header.state);
    }

    version_information->firmware_major = data[0];
    version_information->firmware_minor = data[1];
    version_information->hardware_revision = data[3];
    version_information->shdlc_major = data[5];
    version_information->shdlc_minor = data[6];

    return error;
}

int16_t sen44_read_device_status_register(uint32_t* status_register) {
    struct sensirion_shdlc_rx_header header;
    uint8_t clear_register = 0;
    uint8_t data[5];
    int16_t error;

    error = sensirion_shdlc_xcv(SEN44_ADDR, SEN44_CMD_READ_DEV_STATUS_REG,
                                sizeof(clear_register), &clear_register,
                                sizeof(data), &header, data);
    if (error) {
        return error;
    }

    *status_register =
        (uint32_t)(data[3] << 24 | data[2] << 16 | data[1] << 8 | data[0]);

    return 0;
}

int16_t sen44_reset(void) {
    return sensirion_shdlc_tx(SEN44_ADDR, SEN44_CMD_RESET, 0, (uint8_t*)NULL);
}
