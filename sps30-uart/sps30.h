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

#ifndef SPS30_H
#define SPS30_H

#ifdef __cplusplus
extern "C" {
#endif

#include "sensirion_arch_config.h"

#define SPS30_MAX_SERIAL_LEN 32
#define SPS30_ERR_NOT_ENOUGH_DATA (-1)
#define SPS30_ERR_STATE_MASK (0x100)
#define SPS30_IS_ERR_STATE(err_code) (((err_code) | 0xff) == 0x1ff)
#define SPS30_GET_ERR_STATE(err_code) ((err_code)&0xff)

struct sps30_measurement {
    float mc_1p0;
    float mc_2p5;
    float mc_4p0;
    float mc_10p0;
    float nc_0p5;
    float nc_1p0;
    float nc_2p5;
    float nc_4p0;
    float nc_10p0;
    float typical_particle_size;
};

struct sps30_version_information {
    uint8_t firmware_major;
    uint8_t firmware_minor;
    uint8_t hardware_revision;
    uint8_t shdlc_major;
    uint8_t shdlc_minor;
};

/**
 * sps_get_driver_version() - Return the driver version
 * Return:  Driver version string
 */
const char* sps_get_driver_version(void);

/**
 * sps30_probe() - check if SPS sensor is available and initialize it
 *
 * Return:  0 on success, an error code otherwise
 */
int16_t sps30_probe(void);

/**
 * sps30_get_serial() - retrieve the serial number
 *
 * Note that serial must be discarded when the return code is non-zero.
 *
 * @serial: Memory where the serial number is written into as hex string (zero
 *          terminated). Must be at least SPS_MAX_SERIAL_LEN long.
 * Return:  0 on success, an error code otherwise
 */
int16_t sps30_get_serial(char* serial);

/**
 * sps30_start_measurement() - start measuring
 *
 * Once the measurement is started, measurements are retrievable once per second
 * with sps30_read_measurement.
 *
 * Return:  0 on success, an error code otherwise
 */
int16_t sps30_start_measurement(void);

/**
 * sps30_stop_measurement() - stop measuring
 *
 * Return:  0 on success, an error code otherwise
 */
int16_t sps30_stop_measurement(void);

/**
 * sps30_read_measurement() - read a measurement
 *
 * Read the last measurement.
 *
 * Return:  0 on success, an error code otherwise
 */
int16_t sps30_read_measurement(struct sps30_measurement* measurement);

/**
 * sps30_sleep() - Enter sleep mode with minimum power consumption.
 *
 * Note: This command is only available since firmware version 2.0 and can only
 * be run when the sensor is in idle mode, i.e. after sps30_stop_measurement().
 *
 * Return:          0 on success, an error code otherwise
 */
int16_t sps30_sleep(void);

/**
 * sps30_wake_up() - Wake up from sleep mode and enter idle state.
 *
 * Note: This command is only available since firmware version 2.0 and can only
 * be run when the sensor is in sleep mode, i.e. after sps30_sleep().
 *
 * Return:          0 on success, an error code otherwise
 */
int16_t sps30_wake_up(void);

/**
 * sps30_get_fan_auto_cleaning_interval() - read the current auto-cleaning
 * interval
 *
 * Note that interval_seconds must be discarded when the return code is
 * non-zero.
 *
 * @interval_seconds:   Memory where the interval in seconds is stored
 * Return:              0 on success, an error code otherwise
 */
int16_t sps30_get_fan_auto_cleaning_interval(uint32_t* interval_seconds);

/**
 * sps30_set_fan_auto_cleaning_interval() - set the current auto-cleaning
 * interval
 *
 * @interval_seconds:   Value in seconds used to sets the auto-cleaning interval
 * Return:              0 on success, an error code otherwise
 */
int16_t sps30_set_fan_auto_cleaning_interval(uint32_t interval_seconds);

/**
 * sps30_get_fan_auto_cleaning_interval_days() - convenience function to read
 * the current auto-cleaning interval in days
 *
 * note that the value is simply cut, not rounded or calculated nicely, thus
 * using this method is not precise when used in conjunction with
 * sps30_set_fan_auto_cleaning_interval instead of
 * sps30_set_fan_auto_cleaning_interval_days
 *
 * Note that interval_days must be discarded when the return code is non-zero.
 *
 * @interval_days:  Memory where the interval in days is stored
 * Return:          0 on success, an error code otherwise
 */
int16_t sps30_get_fan_auto_cleaning_interval_days(uint8_t* interval_days);

/**
 * sps30_set_fan_auto_cleaning_interval_days() - convenience function to set the
 * current auto-cleaning interval in days
 *
 * @interval_days:  Value in days used to sets the auto-cleaning interval
 * Return:          0 on success, an error code otherwise
 */
int16_t sps30_set_fan_auto_cleaning_interval_days(uint8_t interval_days);

/**
 * sps30_start_manual_fan_cleaning() - Immediately trigger the fan cleaning
 *
 * Note that this command can only be run when the sensor is in measurement
 * mode, i.e. after sps30_start_measurement() without subsequent
 * sps30_stop_measurement().
 *
 * Return:          0 on success, an error code otherwise
 */
int16_t sps30_start_manual_fan_cleaning(void);

/**
 * sps30_read_version() - Read version information.
 *
 * Return:          0 on success, an error code otherwise
 */
int16_t
sps30_read_version(struct sps30_version_information* version_information);

/**
 * sps30_reset() - reset the SGP30
 *
 * Return:          0 on success, an error code otherwise
 */
int16_t sps30_reset(void);

#ifdef __cplusplus
}
#endif

#endif /* SPS30_H */
