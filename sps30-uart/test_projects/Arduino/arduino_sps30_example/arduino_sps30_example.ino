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
#include <Arduino.h>

#include "sensirion_uart.h"
#include "sps30.h"


void setup() {
    Serial.begin(115200);

    while (!Serial) {
      delay(100);
    }

    // use built-in LED to show errors
    sensirion_uart_open();
}

void loop() {
    struct sps30_measurement measurement;
    int16_t ret;

    while (sps30_probe() != 0) {
        Serial.println("probe failed");
        delay(1000);
    }

    /* start measurement and wait for 10s to ensure the sensor has a
     * stable flow and possible remaining particles are cleaned out */
    if (sps30_start_measurement() != 0) {
        Serial.println("error starting measurement");
    }
    delay(10000);

    while (1) {
        delay(1000);
        ret = sps30_read_measurement(&measurement);

        if (ret < 0) {
          Serial.println("read measurement failed");
        } else {
            if (SPS30_IS_ERR_STATE(ret)) {
              Serial.print("Chip state: ");
              Serial.print(SPS30_GET_ERR_STATE(ret), DEC);
              Serial.println(" - measurements may not be accurate");
            }
            Serial.print("PM 2.5: ");
            Serial.println(measurement.mc_2p5, DEC);
        }
    }

    sps30_stop_measurement();
    sensirion_uart_close();
}
