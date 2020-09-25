#include "sensirion_test_setup.h"
#include "sps30.h"

// Measurement ranges according to datasheet
#define SPS30_MIN_MC 0
#define SPS30_MAX_MC 1000
#define SPS30_MIN_NC 0
#define SPS30_MAX_NC 3000
#define CMD_DELAY_USEC 20000
#define SLEEP_WAKE_UP_DELAY_USEC 5000

#ifndef SENSIRION_UART_TTYDEV
#define SENSIRION_UART_TTYDEV "/dev/ttyUSB0"
#endif

// Less or equal (<=) for float arithmetics with small error (1e-5)
#define LEQ(l, u) ((l) - (u) < 1e-5)

TEST_GROUP (SPS30_Test) {
    void setup() {
        int16_t error;

        error = sensirion_uart_open();
        CHECK_ZERO_TEXT(error, "sensirion_uart_open");
    }

    void teardown() {
        int16_t error;

        error = sps30_reset();
        CHECK_ZERO_TEXT(error, "sps30_reset in test reset");
        sensirion_sleep_usec(CMD_DELAY_USEC);
        sensirion_uart_close();
        CHECK_ZERO_TEXT(error, "sensirion_uart_close");
    }
};

TEST (SPS30_Test, SPS30_probe) {
    int16_t error;

    error = sps30_probe();
    CHECK_ZERO_TEXT(error, "sps30_probe");
    sensirion_sleep_usec(CMD_DELAY_USEC);
}

TEST (SPS30_Test, SPS30_get_driver_version) {
    const char* version = sps_get_driver_version();

    CHECK_ZERO_TEXT(!version, "sps30_get_driver_version");
    printf("sps30_get_driver_version: %s\n", version);
}

TEST (SPS30_Test, SPS30_get_serial) {
    int16_t error;
    char serial[SPS30_MAX_SERIAL_LEN];

    error = sps30_get_serial(serial);
    CHECK_ZERO_TEXT(error, "sps30_get_serial");
    sensirion_sleep_usec(CMD_DELAY_USEC);
    printf("SPS30 serial: %s\n", serial);
}

TEST (SPS30_Test, SPS30_sleep_and_wake_up) {
    int16_t error;

    error = sps30_sleep();
    CHECK_ZERO_TEXT(error, "SPS30_sleep");
    sensirion_sleep_usec(SLEEP_WAKE_UP_DELAY_USEC);
    sensirion_sleep_usec(1000000);  // let sensor sleep for some time
    error = sps30_wake_up();
    CHECK_ZERO_TEXT(error, "SPS30_wake_up");
    sensirion_sleep_usec(SLEEP_WAKE_UP_DELAY_USEC);
}

TEST (SPS30_Test, SPS30_fan_auto_cleaning_interval) {
    int16_t error;
    uint32_t get_interval;
    uint32_t set_interval = 4 * 24 * 60 * 60;  // arbitrary

    error = sps30_set_fan_auto_cleaning_interval(set_interval);
    CHECK_ZERO_TEXT(error, "sps30_set_fan_auto_cleaning_interval");
    sensirion_sleep_usec(CMD_DELAY_USEC);
    error = sps30_get_fan_auto_cleaning_interval(&get_interval);
    CHECK_ZERO_TEXT(error, "sps30_get_fan_auto_cleaning_interval");
    sensirion_sleep_usec(CMD_DELAY_USEC);
    CHECK_EQUAL_TEXT(set_interval, get_interval,
                     "Fan auto cleaning intervals do not match");
}

TEST (SPS30_Test, SPS30_get_fan_auto_cleaning_interval_days) {
    int16_t error;
    uint8_t get_interval_days;
    uint8_t set_interval_days = 4;  // arbitrary

    error = sps30_set_fan_auto_cleaning_interval_days(set_interval_days);
    CHECK_ZERO_TEXT(error, "sps30_set_fan_auto_cleaning_interval_days");
    sensirion_sleep_usec(CMD_DELAY_USEC);
    error = sps30_get_fan_auto_cleaning_interval_days(&get_interval_days);
    CHECK_ZERO_TEXT(error, "sps30_get_fan_auto_cleaning_interval_days");
    sensirion_sleep_usec(CMD_DELAY_USEC);
    CHECK_EQUAL_TEXT(set_interval_days, get_interval_days,
                     "Fan auto cleaning interval days do not match");
}

TEST (SPS30_Test, SPS30_start_manual_fan_cleaning) {
    int16_t error;

    error = sps30_start_manual_fan_cleaning();
    CHECK_ZERO_TEXT(error, "sps30_start_manual_fan_cleaning");
}

TEST (SPS30_Test, SPS30_read_version) {
    int16_t error;
    struct sps30_version_information version_information;

    error = sps30_read_version(&version_information);
    CHECK_ZERO_TEXT(error, "sps30_read_version");
    sensirion_sleep_usec(CMD_DELAY_USEC);
    printf("Firmware_Major: %i\nFirmware_Minor: %i\nHardware_revision: "
           "%i\nSHDLC_Major: %i\nSHDLC_Minor: %i\n",
           version_information.firmware_major,
           version_information.firmware_minor,
           version_information.hardware_revision,
           version_information.shdlc_major, version_information.shdlc_minor);
}

TEST (SPS30_Test, SPS30_reset) {
    int16_t error;

    error = sps30_reset();
    CHECK_ZERO_TEXT(error, "sps30_reset in test reset");
    sensirion_sleep_usec(CMD_DELAY_USEC);
}

TEST (SPS30_Test, SPS30_measurement) {
    int16_t error;
    struct sps30_measurement m;

    error = sps30_start_measurement();
    CHECK_ZERO_TEXT(error, "sps30_start_measurement");
    sensirion_sleep_usec(CMD_DELAY_USEC);

    sensirion_sleep_usec(1000000);  // wait 1 sec for measurement to be ready
    error = sps30_read_measurement(&m);
    CHECK_ZERO_TEXT(error, "sps30_read_measurement");
    sensirion_sleep_usec(CMD_DELAY_USEC);
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
           m.mc_1p0, m.mc_2p5, m.mc_4p0, m.mc_10p0, m.nc_0p5, m.nc_1p0,
           m.nc_2p5, m.nc_4p0, m.nc_10p0, m.typical_particle_size);

    // Check if mass concentration is rising monotonously
    CHECK_TRUE_TEXT(LEQ(SPS30_MIN_MC, m.mc_1p0) && LEQ(m.mc_1p0, m.mc_2p5) &&
                        LEQ(m.mc_2p5, m.mc_4p0) && LEQ(m.mc_4p0, m.mc_10p0) &&
                        LEQ(m.mc_10p0, SPS30_MAX_MC),
                    "Mass concentration not rising monotonously");

    // Check if number concentration is rising monotonously
    CHECK_TRUE_TEXT(LEQ(SPS30_MIN_NC, m.nc_0p5) && LEQ(m.nc_0p5, m.nc_1p0) &&
                        LEQ(m.nc_1p0, m.nc_2p5) && LEQ(m.nc_2p5, m.nc_4p0) &&
                        LEQ(m.nc_4p0, m.nc_10p0) &&
                        LEQ(m.nc_10p0, SPS30_MAX_NC),
                    "Number concentration not rising monotonously");

    error = sps30_stop_measurement();
    CHECK_ZERO_TEXT(error, "sps30_stop_measurement");
    sensirion_sleep_usec(CMD_DELAY_USEC);
}
