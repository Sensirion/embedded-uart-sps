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

#include <stdio.h> // printf
#include <unistd.h> // sleep

#include "sensirion_uart.h"
#include "sps30.h"

/**
 * TO USE CONSOLE OUTPUT (PRINTF) AND WAIT (SLEEP) PLEASE ADAPT THEM TO YOUR PLATFORM
 */
//#define printf(...)
//#define sleep(...)

int main(void)
{
    struct sps30_measurement m;
    char serial[SPS_MAX_SERIAL_LEN];
    uint8_t auto_clean_days = 4;
    uint32_t auto_clean;
    int16_t ret;

    while (sensirion_uart_open() != 0) {
        printf("UART init failed\n");
        sleep(1);
    }

    /* Busy loop for initialization, because the main loop does not work without
     * a sensor.
     */
    while (sps30_probe() != 0) {
        printf("SPS sensor probing failed\n");
        sleep(1);
    }
    printf("SPS sensor probing successful\n");

    ret = sps30_get_serial(serial);
    if (ret)
        printf("error %d reading serial\n", ret);
    else
        printf("SPS Serial: %s\n", serial);

    ret = sps30_get_fan_auto_cleaning_interval(&auto_clean);
    if (ret)
        printf("error %d retrieving the auto-clean interval\n", ret);
    else
        printf("auto-cleaning interval is %d seconds\n", auto_clean);

    ret = sps30_set_fan_auto_cleaning_interval_days(auto_clean_days);
    if (ret)
        printf("error %d setting the auto-clean interval\n", ret);

    ret = sps30_get_fan_auto_cleaning_interval_days(&auto_clean_days);
    if (ret)
        printf("error retrieving the auto-clean interval\n");
    else
        printf("auto-cleaning interval set to %u days\n", auto_clean_days);

    ret = sps30_start_measurement();
    if (ret < 0)
        printf("error starting measurement\n");
    printf("measurements started\n");

    do {
        ret = sps30_read_measurement(&m);
        if (ret < 0) {
            printf("error reading measurement\n");

        } else {
            if (SPS_IS_ERR_STATE(ret)) {
                printf("Chip state: %u - measurements may not be accurate\n",
                        SPS_GET_ERR_STATE(ret));
            }

            printf("measured values:\n"
                   "\t%0.2f pm1.0\n"
                   "\t%0.2f pm2.5\n"
                   "\t%0.2f pm4.0\n"
                   "\t%0.2f pm10.0\n"
                   "\t%0.2f nc0.5\n"
                   "\t%0.2f nc1.0\n"
                   "\t%0.2f nc2.5\n"
                   "\t%0.2f nc4.5\n"
                   "\t%0.2f nc10.0\n"
                   "\t%0.2f typical particle size\n\n",
                   m.mc_1p0, m.mc_2p5, m.mc_4p0, m.mc_10p0,
                   m.nc_0p5, m.nc_1p0, m.nc_2p5, m.nc_4p0, m.nc_10p0,
                   m.typical_particle_size);
        }

        sleep(1);
    } while (1);

    if (sensirion_uart_close() != 0)
        printf("failed to close UART\n");

    return 0;
}
