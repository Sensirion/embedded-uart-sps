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
(sps30-uart-1.1.0.zip for example)

```bash
# on the Raspberry Pi
$ wget https://github.com/Sensirion/embedded-uart-sps/releases/download/1.1.0/sps30-uart-1.1.0.zip
$ unzip sps30-uart-1.1.0.zip
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
$ cd sps30-uart-1.1.0
$ cp sample-implementations/linux/sensirion_uart_implementation.c sensirion_uart_implementation.c
```

Then in `sensirion_uart_implementation.c` change the line with `#define TTYDEV`
to match the device you got from the step before.
```c
#define TTYDEV "/dev/ttyUSB0"
```

## Compile and Run

Now we are ready to compile and run the sample:
```bash
# on the Raspberry Pi
$ cd sps30-uart-1.1.0
$ make
cc -Wall -I. -I. -I. -c -o sensirion_uart_implementation.o sensirion_uart_implementation.c
cc -Wall -I. -I. -I. -c -o sensirion_shdlc.o sensirion_shdlc.c
cc -Wall -I. -I. -I. -c -o sps_git_version.o sps_git_version.c
cc -Wall -I. -I. -I.   -c -o sps30.o sps30.c
cc -Wall -I. -I. -I.  -o sps30_example_usage sensirion_uart_implementation.o sensirion_shdlc.o sps_git_version.o sps30.o   sps30_example_usage.c
$ ./sps30_example_usage
```

You should see the following output:

```
SPS sensor probing successful
SPS Serial: 0674F9B50F9B32F0
auto-cleaning interval is 345600 seconds
auto-cleaning interval set to 4 days
measurements started
error reading measurement
measured values:
	0.00 pm1.0
	0.00 pm2.5
	0.00 pm4.0
	0.00 pm10.0
	-0.00 nc0.5
	0.00 nc1.0
	0.00 nc2.5
	0.00 nc4.5
	0.00 nc10.0
	1.63 typical particle size
```

After a few measurements meaningful values should show up:
```
measured values:
	1.35 pm1.0
	4.25 pm2.5
	6.40 pm4.0
	6.62 pm10.0
	0.93 nc0.5
	6.72 nc1.0
	9.85 nc2.5
	10.23 nc4.5
	10.29 nc10.0
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
