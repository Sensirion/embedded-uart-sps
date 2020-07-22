# Getting Started on the Raspberry Pi 3

This guide assumes you are using the [Raspbian image]. It should work on other
Linux based setups as well, but some details may be different.

We will compile the driver on the Raspberry Pi itself so we assume that the
reader has basic knowledge of the Raspberry Pi, shell access to it and that it
has a connection to the internet.

The shell code examples below use the following convention:

 * Lines starting with `# ` are comments
 * Lines starting with `$ ` are commands that should be executed (without the
   `$` prompt)
 * Other lines are the expected output from a command

## Download Driver

Get the latest release from
https://github.com/Sensirion/embedded-uart-sps/releases/latest
(sps30-uart-3.1.0.zip for example)

```bash
# on the Raspberry Pi
$ wget https://github.com/Sensirion/embedded-uart-sps/releases/download/3.1.0/sps30-uart-3.1.0.zip
$ unzip sps30-uart-3.1.0.zip
```

## Get Port of the Connected Sensor

We need to know the port of the connected sensor. After connecting the sensor
via USB run the following:
```bash
# on the Raspberry Pi
$ dmesg | grep usb
```
This should show a line like this:
```
[   63.545635] usb 1-1.3: FTDI USB Serial Device converter now attached to ttyUSB0
```
This means the serial device will be reachable as `/dev/ttyUSB0`.

## Adapt the Source Code

Copy the file `sample-implementations/linux/sensirion_uart_implementation.c` to
the base folder:

```bash
# on the Raspberry Pi
$ cd sps30-uart-3.1.0
$ cp sample-implementations/linux/sensirion_uart_implementation.c sensirion_uart_implementation.c
```

Then in `sensirion_uart_implementation.c` change the line with
`#define SENSIRION_UART_TTYDEV` to match the device you got from the step
before.
```c
#define SENSIRION_UART_TTYDEV "/dev/ttyUSB0"
```

## Compile and Run

Now we are ready to compile the example:
```bash
# on the Raspberry Pi
$ cd sps30-uart-3.1.0
$ make
rm -f sps30_example_usage
cc -Os -Wall -fstrict-aliasing -Wstrict-aliasing=1 -Wsign-conversion -fPIC -I. -I. -I.  -o sps30_example_usage ./sensirion_arch_config.h ./sensirion_uart.h ./sensirion_shdlc.h ./sensirion_shdlc.c ./sps_git_version.h ./sps_git_version.c ./sps30.h ./sps30.c ./sensirion_uart_implementation.c ./sps30_example_usage.c
```

When you run the example you should see the following output:

```
$ ./sps30_example_usage
SPS30 sensor probing successful
FW: 2.2 HW: 7, SHDLC: 2.0
SPS30 Serial: ECBBEEC8D8D650F3
measurements started
measured values:
	1.27 pm1.0
	1.34 pm2.5
	1.33 pm4.0
	1.23 pm10.0
	8.66 nc0.5
	10.11 nc1.0
	10.15 nc2.5
	10.15 nc4.5
	10.15 nc10.0
	0.47 typical particle size
```

After a minute measurements will be stopped and the sensor will be put to sleep
(if the firmware version is newer then 2.0):
```
measured values:
	1.25 pm1.0
	1.32 pm2.5
	1.30 pm4.0
	1.21 pm10.0
	8.50 nc0.5
	9.92 nc1.0
	9.96 nc2.5
	9.96 nc4.5
	9.96 nc10.0
	0.48 typical particle size

No measurements for 1 minute
```

After a minute of sleep measurements will be started again:
```
measurements started
error reading measurement
measured values:
	2.23 pm1.0
	7.80 pm2.5
	12.24 pm4.0
	13.36 pm10.0
	3.90 nc0.5
	12.34 nc1.0
	17.45 nc2.5
	18.54 nc4.5
	18.81 nc10.0
	1.63 typical particle size
```

## Troubleshooting

### `SPS sensor probing failed`

If you get the this error message verify that you copied the Linux sample
implementation and that you have the correct device set. See [Get Port of the
Connected Sensor](#get-port-of-the-connected-sensor)

### `Error opening UART. Ensure it's not otherwise used` / `UART init failed`

This happens mostly when the UART device does not exist. See [Get Port of the
Connected Sensor](#get-port-of-the-connected-sensor)

**Note:** It may happen if you unplug and replug the sensor that it gets
another device assigned (`/dev/ttyUSB1` for example). In that case you either
need to change the code and compile again or try to replug it and check if the
device changed back.

[Raspbian image]: https://www.raspberrypi.org/downloads/raspbian/
