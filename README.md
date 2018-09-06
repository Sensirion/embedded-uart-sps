# embedded-uart-sps
This repository contains the embedded driver sources for Sensirion's
SPS product line.

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
1. Step into your desired directory (e.g.: `release/sps30`)
2. Adjust sensirion\_arch\_config.h if the `<stdint.h>` header is not available
3. Implement necessary functions in `*_implementation.c`
4. make

---