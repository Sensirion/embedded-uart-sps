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

#include "sps30.h"
#include "sensirion_shdlc.h"
#include "sensirion_uart.h"
#include "sps_git_version.h"

#define SPS30_ADDR 0x00
#define SPS30_CMD_START_MEASUREMENT 0x00
#define SPS30_CMD_STOP_MEASUREMENT 0x01
#define SPS30_SUBCMD_MEASUREMENT_START                                         \
    { 0x01, 0x03 }
#define SPS30_CMD_READ_MEASUREMENT 0x03
#define SPS30_CMD_SLEEP 0x10
#define SPS30_CMD_WAKE_UP 0x11
#define SPS30_CMD_FAN_CLEAN_INTV 0x80
#define SPS30_CMD_FAN_CLEAN_INTV_LEN 5
#define SPS30_SUBCMD_READ_FAN_CLEAN_INTV 0x00
#define SPS30_CMD_START_FAN_CLEANING 0x56
#define SPS30_CMD_DEV_INFO 0xd0
#define SPS30_CMD_DEV_INFO_SUBCMD_GET_SERIAL                                   \
    { 0x03 }
#define SPS30_CMD_READ_VERSION 0xd1
#define SPS30_CMD_RESET 0xd3
#define SPS30_ERR_STATE(state) (SPS30_ERR_STATE_MASK | (state))

const char *sps_get_driver_version(void) {
    return SPS_DRV_VERSION_STR;
}

int16_t sps30_probe(void) {
    char serial[SPS30_MAX_SERIAL_LEN];
    // Try to wake up, but ignore failure if it is not in sleep mode
    (void)sps30_wake_up();
    int16_t ret = sps30_get_serial(serial);

    return ret;
}

int16_t sps30_get_serial(char *serial) {
    struct sensirion_shdlc_rx_header header;
    uint8_t param_buf[] = SPS30_CMD_DEV_INFO_SUBCMD_GET_SERIAL;
    int16_t ret;

    ret = sensirion_shdlc_xcv(SPS30_ADDR, SPS30_CMD_DEV_INFO, sizeof(param_buf),
                              param_buf, SPS30_MAX_SERIAL_LEN, &header,
                              (uint8_t *)serial);
    if (ret < 0)
        return ret;

    if (header.state)
        return SPS30_ERR_STATE(header.state);

    return 0;
}

int16_t sps30_start_measurement(void) {
    struct sensirion_shdlc_rx_header header;
    uint8_t param_buf[] = SPS30_SUBCMD_MEASUREMENT_START;

    return sensirion_shdlc_xcv(SPS30_ADDR, SPS30_CMD_START_MEASUREMENT,
                               sizeof(param_buf), param_buf, 0, &header, NULL);
}

int16_t sps30_stop_measurement(void) {
    struct sensirion_shdlc_rx_header header;

    return sensirion_shdlc_xcv(SPS30_ADDR, SPS30_CMD_STOP_MEASUREMENT, 0, NULL,
                               0, &header, NULL);
}

int16_t sps30_read_measurement(struct sps30_measurement *measurement) {
    struct sensirion_shdlc_rx_header header;
    int16_t ret;
    uint16_t idx;
    union {
        uint16_t u16_value[2];
        uint32_t u32_value;
        float f32_value;
    } val, data[10];

    ret = sensirion_shdlc_xcv(SPS30_ADDR, SPS30_CMD_READ_MEASUREMENT, 0, NULL,
                              sizeof(data), &header, (uint8_t *)data);
    if (ret)
        return ret;

    if (header.data_len != sizeof(data))
        return SPS30_ERR_NOT_ENOUGH_DATA;

    idx = 0;
    val.u32_value = be32_to_cpu(data[idx].u32_value);
    measurement->mc_1p0 = val.f32_value;
    ++idx;
    val.u32_value = be32_to_cpu(data[idx].u32_value);
    measurement->mc_2p5 = val.f32_value;
    ++idx;
    val.u32_value = be32_to_cpu(data[idx].u32_value);
    measurement->mc_4p0 = val.f32_value;
    ++idx;
    val.u32_value = be32_to_cpu(data[idx].u32_value);
    measurement->mc_10p0 = val.f32_value;
    ++idx;
    val.u32_value = be32_to_cpu(data[idx].u32_value);
    measurement->nc_0p5 = val.f32_value;
    ++idx;
    val.u32_value = be32_to_cpu(data[idx].u32_value);
    measurement->nc_1p0 = val.f32_value;
    ++idx;
    val.u32_value = be32_to_cpu(data[idx].u32_value);
    measurement->nc_2p5 = val.f32_value;
    ++idx;
    val.u32_value = be32_to_cpu(data[idx].u32_value);
    measurement->nc_4p0 = val.f32_value;
    ++idx;
    val.u32_value = be32_to_cpu(data[idx].u32_value);
    measurement->nc_10p0 = val.f32_value;
    ++idx;
    val.u32_value = be32_to_cpu(data[idx].u32_value);
    measurement->typical_particle_size = val.f32_value;

    if (header.state)
        return SPS30_ERR_STATE(header.state);

    return 0;
}

int16_t sps30_sleep(void) {
    struct sensirion_shdlc_rx_header header;

    return sensirion_shdlc_xcv(SPS30_ADDR, SPS30_CMD_SLEEP, 0, NULL, 0, &header,
                               NULL);
}

int16_t sps30_wake_up(void) {
    struct sensirion_shdlc_rx_header header;
    int16_t ret;
    const uint8_t data = 0xFF;

    ret = sensirion_uart_tx(1, &data);
    if (ret < 0) {
        return ret;
    }
    return sensirion_shdlc_xcv(SPS30_ADDR, SPS30_CMD_WAKE_UP, 0, NULL, 0,
                               &header, NULL);
}

int16_t sps30_get_fan_auto_cleaning_interval(uint32_t *interval_seconds) {
    struct sensirion_shdlc_rx_header header;
    uint8_t tx_data[] = {SPS30_SUBCMD_READ_FAN_CLEAN_INTV};
    int16_t ret;

    ret = sensirion_shdlc_xcv(
        SPS30_ADDR, SPS30_CMD_FAN_CLEAN_INTV, sizeof(tx_data), tx_data,
        sizeof(*interval_seconds), &header, (uint8_t *)interval_seconds);
    if (ret < 0)
        return ret;

    *interval_seconds = be32_to_cpu(*interval_seconds);

    if (header.state)
        return SPS30_ERR_STATE(header.state);

    return 0;
}

int16_t sps30_set_fan_auto_cleaning_interval(uint32_t interval_seconds) {
    struct sensirion_shdlc_rx_header header;
    uint8_t ix;
    uint8_t cleaning_command[SPS30_CMD_FAN_CLEAN_INTV_LEN];
    uint32_t value = be32_to_cpu(interval_seconds);

    cleaning_command[0] = SPS30_SUBCMD_READ_FAN_CLEAN_INTV;
    for (ix = 0; ix < sizeof(uint32_t); ix++)
        cleaning_command[ix + 1] = (uint8_t)(value >> (8 * ix));
    return sensirion_shdlc_xcv(
        SPS30_ADDR, SPS30_CMD_FAN_CLEAN_INTV, sizeof(cleaning_command),
        (const uint8_t *)cleaning_command, sizeof(interval_seconds), &header,
        (uint8_t *)&interval_seconds);
}

int16_t sps30_get_fan_auto_cleaning_interval_days(uint8_t *interval_days) {
    int16_t ret;
    uint32_t interval_seconds;

    ret = sps30_get_fan_auto_cleaning_interval(&interval_seconds);
    if (ret < 0)
        return ret;

    *interval_days = interval_seconds / (24 * 60 * 60);
    return ret;
}

int16_t sps30_set_fan_auto_cleaning_interval_days(uint8_t interval_days) {
    return sps30_set_fan_auto_cleaning_interval((uint32_t)interval_days * 24 *
                                                60 * 60);
}

int16_t sps30_start_manual_fan_cleaning(void) {
    struct sensirion_shdlc_rx_header header;

    return sensirion_shdlc_xcv(SPS30_ADDR, SPS30_CMD_START_FAN_CLEANING, 0,
                               NULL, 0, &header, NULL);
}

int16_t
sps30_read_version(struct sps30_version_information *version_information) {
    struct sensirion_shdlc_rx_header header;
    int16_t error;
    uint8_t data[7];

    error = sensirion_shdlc_xcv(SPS30_ADDR, SPS30_CMD_READ_VERSION, 0, NULL,
                                sizeof(data), &header, data);
    if (error) {
        return error;
    }

    if (header.data_len != sizeof(data)) {
        return SPS30_ERR_NOT_ENOUGH_DATA;
    }

    if (header.state) {
        return SPS30_ERR_STATE(header.state);
    }

    version_information->firmware_major = data[0];
    version_information->firmware_minor = data[1];
    version_information->hardware_revision = data[3];
    version_information->shdlc_major = data[5];
    version_information->shdlc_minor = data[6];

    return error;
}

int16_t sps30_reset(void) {
    return sensirion_shdlc_tx(SPS30_ADDR, SPS30_CMD_RESET, 0, NULL);
}
