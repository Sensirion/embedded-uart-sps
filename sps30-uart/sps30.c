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

#include "sensirion_shdlc.h"
#include "sps_git_version.h"
#include "sps30.h"

#define SPS_ADDR 0x00
#define SPS_CMD_START_MEASUREMENT 0x00
#define SPS_CMD_STOP_MEASUREMENT 0x01
#define SPS_SUBCMD_MEASUREMENT_START {0x01, 0x03}
#define SPS_CMD_READ_MEASUREMENT 0x03
#define SPS_CMD_READ_FAN_SPEED 0x40
#define SPS_CMD_FAN_CLEAN_INTV 0x80
#define SPS_SUBCMD_READ_FAN_CLEAN_INTV 0x00
#define SPS_CMD_DEV_INFO 0xd0
#define SPS_CMD_DEV_INFO_SUBCMD_GET_SERIAL {0x03}
#define SPS_CMD_RESET 0xd3
#define SPS_ERR_STATE(state) (SPS_ERR_STATE_MASK | (state))
#define SPS_CMD_FAN_CLEAN_LENGTH 5

const char *sps_get_driver_version()
{
    return SPS_DRV_VERSION_STR;
}

s16 sps30_probe() {
    char serial[SPS_MAX_SERIAL_LEN];
    s16 ret = sps30_get_serial(serial);

    return ret;
}

s16 sps30_get_serial(char *serial) {
    struct sensirion_shdlc_rx_header header;
    u8 param_buf[] = SPS_CMD_DEV_INFO_SUBCMD_GET_SERIAL;
    s16 ret;

    ret = sensirion_shdlc_xcv(SPS_ADDR, SPS_CMD_DEV_INFO,
                              sizeof(param_buf), param_buf,
                              SPS_MAX_SERIAL_LEN, &header, (u8 *)serial);
    if (ret < 0)
        return ret;

    if (header.state)
        return SPS_ERR_STATE(header.state);

    return 0;
}

s16 sps30_start_measurement() {
    u8 param_buf[] = SPS_SUBCMD_MEASUREMENT_START;

    return sensirion_shdlc_tx(SPS_ADDR, SPS_CMD_START_MEASUREMENT,
                              sizeof(param_buf), param_buf);
}

s16 sps30_stop_measurement() {
    return sensirion_shdlc_tx(SPS_ADDR, SPS_CMD_STOP_MEASUREMENT, 0, NULL);
}

s16 sps30_read_measurement(struct sps30_measurement *measurement) {
    struct sensirion_shdlc_rx_header header;
    f32 data[10];
    s16 ret;
    u16 idx;
    u32 *u32_data = (u32 *)data;
    u32 tmp;

    ret = sensirion_shdlc_xcv(SPS_ADDR, SPS_CMD_READ_MEASUREMENT, 0, NULL,
                              sizeof(data), &header, (u8 *)data);
    if (ret)
        return ret;

    if (header.data_len != sizeof(data))
        return SPS_ERR_NOT_ENOUGH_DATA;

    idx = 0;
    tmp = be32_to_cpu(u32_data[idx]);
    measurement->mc_1p0 = *(f32 *)&tmp;
    ++idx;
    tmp = be32_to_cpu(u32_data[idx]);
    measurement->mc_2p5 = *(f32 *)&tmp;
    ++idx;
    tmp = be32_to_cpu(u32_data[idx]);
    measurement->mc_4p0 = *(f32 *)&tmp;
    ++idx;
    tmp = be32_to_cpu(u32_data[idx]);
    measurement->mc_10p0 = *(f32 *)&tmp;
    ++idx;
    tmp = be32_to_cpu(u32_data[idx]);
    measurement->nc_0p5 = *(f32 *)&tmp;
    ++idx;
    tmp = be32_to_cpu(u32_data[idx]);
    measurement->nc_1p0 = *(f32 *)&tmp;
    ++idx;
    tmp = be32_to_cpu(u32_data[idx]);
    measurement->nc_2p5 = *(f32 *)&tmp;
    ++idx;
    tmp = be32_to_cpu(u32_data[idx]);
    measurement->nc_4p0 = *(f32 *)&tmp;
    ++idx;
    tmp = be32_to_cpu(u32_data[idx]);
    measurement->nc_10p0 = *(f32 *)&tmp;
    ++idx;
    tmp = be32_to_cpu(u32_data[idx]);
    measurement->typical_particle_size = *(f32 *)&tmp;
    ++idx;

    if (header.state)
        return SPS_ERR_STATE(header.state);

    return 0;
}

s16 sps30_read_fan_speed(u16 *fan_rpm) {
    struct sensirion_shdlc_rx_header header;
    s16 ret;

    ret = sensirion_shdlc_xcv(SPS_ADDR, SPS_CMD_READ_FAN_SPEED, 0, NULL,
                              sizeof(*fan_rpm), &header, (u8 *)fan_rpm);
    if (ret < 0)
        return ret;

    *fan_rpm = be16_to_cpu(*fan_rpm);
    if (header.state)
        return SPS_ERR_STATE(header.state);

    return 0;
}

s16 sps30_get_fan_auto_cleaning_interval(u32 *interval_seconds) {
    struct sensirion_shdlc_rx_header header;
    u8 tx_data[] = {SPS_SUBCMD_READ_FAN_CLEAN_INTV};
    s16 ret;

    ret = sensirion_shdlc_xcv(SPS_ADDR, SPS_CMD_FAN_CLEAN_INTV, sizeof(tx_data),
                              tx_data, sizeof(*interval_seconds), &header,
                              (u8 *)interval_seconds);
    if (ret < 0)
        return ret;

    *interval_seconds = be32_to_cpu(*interval_seconds);

    if (header.state)
        return SPS_ERR_STATE(header.state);

    return 0;
}

s16 sps30_set_fan_auto_cleaning_interval(u32 interval_seconds) {
    struct sensirion_shdlc_rx_header header;
    u8 ix;
    u8 cleaning_command[SPS_CMD_FAN_CLEAN_LENGTH];
    u32 value = be32_to_cpu(interval_seconds);

    cleaning_command[0] = SPS_SUBCMD_READ_FAN_CLEAN_INTV;
    for (ix = 0; ix < sizeof(u32); ix++)
        cleaning_command[ix + 1] = (u8)(value >> (8 * ix));
    return sensirion_shdlc_xcv(SPS_ADDR, SPS_CMD_FAN_CLEAN_INTV,
                               sizeof(cleaning_command),
                               (const u8 *)cleaning_command,
                               sizeof(interval_seconds), &header,
                               (u8 *)&interval_seconds);
}

s16 sps30_get_fan_auto_cleaning_interval_days(u8 *interval_days) {
    s16 ret;
    u32 interval_seconds;

    ret = sps30_get_fan_auto_cleaning_interval(&interval_seconds);
    if (ret < 0)
        return ret;

    *interval_days = interval_seconds / (24 * 60 * 60);
    return ret;
}

s16 sps30_set_fan_auto_cleaning_interval_days(u8 interval_days) {
    return sps30_set_fan_auto_cleaning_interval((u32)interval_days *
                                                24 * 60 * 60);
}

s16 sps30_reset() {
    return sensirion_shdlc_tx(SPS_ADDR, SPS_CMD_RESET, 0, NULL);
}
