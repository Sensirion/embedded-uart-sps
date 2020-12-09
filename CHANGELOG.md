# Changelog

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

## [3.3.0] - 2020-12-09

 * [`added`]   SEN44 Support

## [3.2.0] - 2020-10-20

 * [`added`]   Add conversion functions for more types to `sensirion_shdlc.*`.
 * [`changed`] Changed wrongly named function `sensirion_uart_release` in sample
               implementation for linux to `sensirion_uart_close` as defined in
               the headerfile `sensirion_uart.h`.
 * [`changed`] Use configuration independent endianness conversions: no need to
               define `SENSIRION_BIG_ENDIAN` anymore.
 * [`changed`] Allow to override the TTY device to use in the Linux UART sample
               implementation.

## [3.1.0] - 2020-07-03

 * [`added`]   Get version information with `sps30_read_version`
 * [`added`]   Add sleep / wake-up support (FW version >= 2.0)

## [3.0.1] - 2020-07-01

 * [`changed`] Distinguishable return value when an SHDLC frame is longer than
               expected
 * [`fixed`]   Fix function declarations to fix compilation on some platforms
 * [`changed`] Don't use `float32_t` typedef but `float` directly

## [3.0.0] - 2019-11-05

 * [`added`]   Multiple sensors API `sensirion_uart_select_port`
 * [`added`]   Multiple sensors sample implementation for arduino MKR0
 * [`changed`] Split out `default_config.inc` from Makefile to configure paths
               and CFLAGS
 * [`fixed`]   Fix strict-aliasing ans sign-conversion compiler warnings
 * [`added`]   instantly start manual fan cleaning with
               `sps30_start_manual_fan_cleaning()`
 * [`changed`] rename `SPS_*` constants to `SPS30_`

## [2.0.0] - 2019-05-14

 * [`added`]   `.clang-format` Style-file for clang-format and helper targets
               `make style-fix` and `make style-check` (#36)
 * [`changed`] Use stdint types (e.g. `uint16_t` instead of `u16`)

## [1.1.1] - 2019-04-11

 * [`fixed`]   Rebuild when `sps30_example_usage.c` changed (#30)
 * [`fixed`]   Fix byte-stuffing issues resulting in commands not being accepted
               by the sensor or values being ignored (#32)

## [1.1.0] - 2019-03-07

 * [`added`]   `sps_get_driver_version()` (#12)
 * [`fixed`]   Send correct command in `sps30_stop_measurement()` (#23)

## [1.0.0] - 2018-11-30

 * [`changed`] Rename driver to sps30-uart to avoid confusion with sps30-i2c (#9)

## [0.9.1] - 2018-09-28

 * [`added`]   Support compiling with C++ (#6)
 * [`added`]   Add Arduino examples (#6)

## [0.9.0] - 2018-09-20

 * Initial SHDLC UART release
 * Initial SPS30 release

[Unreleased]: https://github.com/Sensirion/embedded-uart-sps/compare/3.3.0...master
[3.3.0]: https://github.com/Sensirion/embedded-uart-sps/compare/3.2.0...3.3.0
[3.2.0]: https://github.com/Sensirion/embedded-uart-sps/compare/3.1.0...3.2.0
[3.1.0]: https://github.com/Sensirion/embedded-uart-sps/compare/3.0.1...3.1.0
[3.0.1]: https://github.com/Sensirion/embedded-uart-sps/compare/3.0.0...3.0.1
[3.0.0]: https://github.com/Sensirion/embedded-uart-sps/compare/2.0.0...3.0.0
[2.0.0]: https://github.com/Sensirion/embedded-uart-sps/compare/1.1.1...2.0.0
[1.1.1]: https://github.com/Sensirion/embedded-uart-sps/compare/1.1.0...1.1.1
[1.1.0]: https://github.com/Sensirion/embedded-uart-sps/compare/1.0.0...1.1.0
[1.0.0]: https://github.com/Sensirion/embedded-uart-sps/compare/0.9.1...1.0.0
[0.9.1]: https://github.com/Sensirion/embedded-uart-sps/compare/0.9.0...0.9.1
[0.9.0]: https://github.com/Sensirion/embedded-uart-sps/releases/tag/0.9.0
