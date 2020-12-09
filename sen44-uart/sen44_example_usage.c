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

#include <stdio.h>  // printf

#include "sen44.h"
#include "sensirion_uart.h"

/**
 * TO USE CONSOLE OUTPUT (PRINTF) AND WAIT (SLEEP) PLEASE ADAPT THEM TO YOUR
 * PLATFORM
 */
//#define printf(...)

int main(void) {
    struct sen44_measurement m;
    struct sen44_version_information version_information;
    char serial[SEN44_MAX_SERIAL_LEN];
    uint32_t device_register;
    int16_t error;

    while (sensirion_uart_open() != 0) {
        printf("UART init failed\n");
        sensirion_sleep_usec(1000000); /* sleep for 1s */
    }

    /* Busy loop for initialization, because the main loop does not work without
     * a sensor.
     */
    while (sen44_probe() != 0) {
        printf("SEN44 sensor probing failed\n");
        sensirion_sleep_usec(1000000); /* sleep for 1s */
    }
    printf("SEN44 sensor probing successful\n");

    error = sen44_get_serial(serial);
    if (error)
        printf("error %d reading serial\n", error);
    else
        printf("SEN44 Serial: %s\n", serial);

    error = sen44_read_device_status_register(&device_register);
    if (error) {
        printf("error %d reading device register\n", error);
    } else {
        printf("SEN44 Device Register: %04x\n", device_register);
    }

    error = sen44_read_version(&version_information);
    if (error) {
        printf("error %d reading version information\n", error);
    } else {
        printf("FW: %u.%u HW: %u, SHDLC: %u.%u\n",
               version_information.firmware_major,
               version_information.firmware_minor,
               version_information.hardware_revision,
               version_information.shdlc_major,
               version_information.shdlc_minor);
    }

    error = sen44_start_measurement();
    if (error < 0)
        printf("error starting measurement\n");
    printf("measurements started\n");

    do {
        error = sen44_read_measurement(&m);
        if (error < 0) {
            printf("error reading measurement\n");

        } else {
            if (SEN44_IS_ERR_STATE(error)) {
                printf("Chip state: %u - measurements may not be accurate\n",
                       SEN44_GET_ERR_STATE(error));
            }

            printf("measured values:\n"
                   "\t%i pm1.0\n"
                   "\t%i pm2.5\n"
                   "\t%i pm4.0\n"
                   "\t%i pm10.0\n"
                   "\t%.1f VOC Index\n"
                   "\t%.2f degree Celsius\n"
                   "\t%.2f %%RH\n",
                   m.mc_1p0, m.mc_2p5, m.mc_4p0, m.mc_10p0, m.voc_index / 10.0,
                   m.ambient_temperature / 200.0, m.ambient_humidity / 100.0);
        }

        sensirion_sleep_usec(1000000); /* sleep for 1s */
    } while (1);

    if (sensirion_uart_close() != 0)
        printf("failed to close UART\n");

    return 0;
}
