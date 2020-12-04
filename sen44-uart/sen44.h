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

#ifndef SEN44_H
#define SEN44_H

#ifdef __cplusplus
extern "C" {
#endif

#include "sensirion_arch_config.h"

#define SEN44_MAX_SERIAL_LEN 32
#define SEN44_ERR_NOT_ENOUGH_DATA (-1)
#define SEN44_ERR_STATE_MASK (0x100)
#define SEN44_IS_ERR_STATE(err_code) (((err_code) | 0xff) == 0x1ff)
#define SEN44_GET_ERR_STATE(err_code) ((err_code)&0xff)

struct sen44_measurement {
    uint16_t mc_1p0;
    uint16_t mc_2p5;
    uint16_t mc_4p0;
    uint16_t mc_10p0;
    int16_t voc_index;
    int16_t ambient_temperature;
    int16_t ambient_humidity;
};

struct sen44_version_information {
    uint8_t firmware_major;
    uint8_t firmware_minor;
    uint8_t hardware_revision;
    uint8_t shdlc_major;
    uint8_t shdlc_minor;
};

/**
 * sen44_get_driver_version() - Return the driver version
 * @return Driver version string
 */
const char* sen44_get_driver_version(void);

/**
 * sen44_probe() - check if SEN44 sensor is available and initialize it
 *
 * @return 0 on success, an error code otherwise
 */
int16_t sen44_probe(void);

/**
 * sen44_get_serial() - retrieve the serial number
 *
 * Note that serial must be discarded when the return code is non-zero.
 *
 * @param serial Memory where the serial number is written into as hex string
 * (zero terminated). Must be at least SPS_MAX_SERIAL_LEN long.
 * @return 0 on success, an error code otherwise
 */
int16_t sen44_get_serial(char* serial);

/**
 * sen44_start_measurement() - start measuring
 *
 * Once the measurement is started, measurements are retrievable once per second
 * with sen44_read_measurement.
 *
 * @return 0 on success, an error code otherwise
 */
int16_t sen44_start_measurement(void);

/**
 * sen44_stop_measurement() - stop measuring
 *
 * @return 0 on success, an error code otherwise
 */
int16_t sen44_stop_measurement(void);

/**
 * sen44_read_measurement() - read a measurement
 *
 * Read the last measurement.
 *
 * Note that the returned variable voc_index is scaled up by 10, the variable
 * ambient_humidity by 100 and the variable ambient_temperature by 200.
 *
 * @return 0 on success, an error code otherwise
 */
int16_t sen44_read_measurement(struct sen44_measurement* measurement);

/**
 * sen44_read_version() - Read version information.
 *
 * @return 0 on success, an error code otherwise
 */
int16_t
sen44_read_version(struct sen44_version_information* version_information);

/**
 * sen44_read_device_status_register() - Use this command to read the Device
 * Status Register.
 *
 * @param device_register Device Status Register
 * @return 0 on success, an error code otherwise
 */
int16_t sen44_read_device_status_register(uint32_t* device_register);

/**
 * sen44_reset() - reset the SEN44
 *
 * @return 0 on success, an error code otherwise
 */
int16_t sen44_reset(void);

#ifdef __cplusplus
}
#endif

#endif /* SEN44_H */
