# embedded-uart-sps [![CircleCI](https://circleci.com/gh/Sensirion/embedded-uart-sps.svg?style=shield)](https://circleci.com/gh/Sensirion/embedded-uart-sps) [![GitHub license](https://img.shields.io/badge/license-BSD3-blue.svg)](https://raw.githubusercontent.com/Sensirion/embedded-uart-sps/master/LICENSE)
This repository contains the embedded UART driver sources for Sensirion's
SPS product line.

The i2c driver of the SPS is available in the
[embedded-sps](https://github.com/Sensirion/embedded-sps) repository.

## Clone this repository
```
git clone https://github.com/Sensirion/embedded-uart-sps.git
```

## Repository content
* `embedded-uart-common` submodule repository for common UART HAL
* `sps30` SPS30 driver

## Collecting resources
```
make release
```
This will create a release folder
with the necessary driver files in it, including a Makefile. That way, you have
just ONE folder with all the sources ready to build your driver for your
platform.

## Files to adjust (from embedded-uart-common)
You only need to touch the following files:

* `sensirion_arch_config.h` architecture specifics, you need to specify the
  integer sizes
* `sensirion_uart_implementation.c` functions for UART communication
  Alternatively ready-to-use implementations are available in the
  `sample-implementations` folder

## Building the driver
1. Step into your desired directory (e.g.: `release/sps30-uart`)
2. Adjust sensirion\_arch\_config.h if the `<stdint.h>` header is not available
3. Implement necessary functions in `*_implementation.c`
4. make

---
