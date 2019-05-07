# Changelog

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

 * `[changed]` switch from `s8`, ..., `u64` style types to `int8_t`, ...,
               `uint64_t` (#35)

## [1.1.1] - 2019-04-11

 * `[fixed]` Rebuild when `sps30_example_usage.c` changed (#30)
 * `[fixed]` Fix byte-stuffing issues resulting in commands not being accepted
             by the sensor or values being ignored (#32)

## [1.1.0] - 2019-03-07

 * `[added]` `sps_get_driver_version()` (#12)
 * `[fixed]` Send correct command in `sps30_stop_measurement()` (#23)

## [1.0.0] - 2018-11-30

 * `[changed]` Rename driver to sps30-uart to avoid confusion with sps30-i2c (#9)

## [0.9.1] - 2018-09-28

 * `[added]` Support compiling with C++ (#6)
 * `[added]` Add Arduino examples (#6)

## [0.9.0] - 2018-09-20

 * Initial SHDLC UART release
 * Initial SPS30 release

[Unreleased]: https://github.com/Sensirion/embedded-uart-sps/compare/1.1.1...master
[1.1.1]: https://github.com/Sensirion/embedded-uart-sps/compare/1.1.0...1.1.1
[1.1.0]: https://github.com/Sensirion/embedded-uart-sps/compare/1.0.0...1.1.0
[1.0.0]: https://github.com/Sensirion/embedded-uart-sps/compare/0.9.1...1.0.0
[0.9.1]: https://github.com/Sensirion/embedded-uart-sps/compare/0.9.0...0.9.1
[0.9.0]: https://github.com/Sensirion/embedded-uart-sps/releases/tag/0.9.0
