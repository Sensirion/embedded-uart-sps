#include "sen44.h"
#include "sensirion_test_setup.h"

#define CMD_DELAY_USEC 20000

#ifndef SENSIRION_UART_TTYDEV
#define SENSIRION_UART_TTYDEV "/dev/ttyUSB0"
#endif

TEST_GROUP (SEN44_Test) {
    void setup() {
        int16_t error;

        error = sensirion_uart_open();
        CHECK_ZERO_TEXT(error, "sensirion_uart_open");
        sensirion_sleep_usec(
            CMD_DELAY_USEC);  // WORKAROUND to ensure open works correctly
    }

    void teardown() {
        int16_t error;

        error = sen44_reset();
        CHECK_ZERO_TEXT(error, "sen44_reset in teardown");
        sensirion_sleep_usec(CMD_DELAY_USEC);
        error = sensirion_uart_close();
        CHECK_ZERO_TEXT(error, "sensirion_uart_close");
    }
};

TEST (SEN44_Test, SEN44_probe) {
    int16_t error;

    error = sen44_probe();
    CHECK_ZERO_TEXT(error, "sen44_probe");
    sensirion_sleep_usec(CMD_DELAY_USEC);
}

TEST (SEN44_Test, SEN44_get_driver_version) {
    const char* version = sen44_get_driver_version();

    CHECK_ZERO_TEXT(!version, "sen44_get_driver_version");
    printf("sen44_get_driver_version: %s\n", version);
}

TEST (SEN44_Test, SEN44_get_serial) {
    int16_t error;
    char serial[SEN44_MAX_SERIAL_LEN];

    error = sen44_get_serial(serial);
    CHECK_ZERO_TEXT(error, "sen44_get_serial");
    sensirion_sleep_usec(CMD_DELAY_USEC);
    printf("SEN44 serial: %s\n", serial);
}

TEST (SEN44_Test, SEN44_read_version) {
    int16_t error;
    struct sen44_version_information version_information;

    error = sen44_read_version(&version_information);
    CHECK_ZERO_TEXT(error, "sen44_read_version");
    sensirion_sleep_usec(CMD_DELAY_USEC);
    printf("Firmware_Major: %i\nFirmware_Minor: %i\nHardware_revision: "
           "%i\nSHDLC_Major: %i\nSHDLC_Minor: %i\n",
           version_information.firmware_major,
           version_information.firmware_minor,
           version_information.hardware_revision,
           version_information.shdlc_major, version_information.shdlc_minor);
}

TEST (SEN44_Test, SEN44_read_device_status_register) {
    int16_t error;
    uint32_t device_register;

    error = sen44_read_device_status_register(&device_register);
    CHECK_ZERO_TEXT(error, "sen44_read_device_status_register");
    sensirion_sleep_usec(CMD_DELAY_USEC);
    printf("SEN44 Device Register: %04x\n", device_register);
}

TEST (SEN44_Test, SEN44_reset) {
    int16_t error;

    error = sen44_reset();
    CHECK_ZERO_TEXT(error, "sen44_reset in test reset");
    sensirion_sleep_usec(CMD_DELAY_USEC);
}

TEST (SEN44_Test, SEN44_measurement) {
    int16_t error;
    struct sen44_measurement m;

    error = sen44_start_measurement();
    CHECK_ZERO_TEXT(error, "sen44_start_measurement");
    sensirion_sleep_usec(CMD_DELAY_USEC);

    sensirion_sleep_usec(1000000);  // wait 1 sec for measurement to be ready
    error = sen44_read_measurement(&m);
    CHECK_ZERO_TEXT(error, "sen44_read_measurement");
    sensirion_sleep_usec(CMD_DELAY_USEC);
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

    error = sen44_stop_measurement();
    CHECK_ZERO_TEXT(error, "sen44_stop_measurement");
    sensirion_sleep_usec(CMD_DELAY_USEC);
}
