sensirion_uart_dir := ../embedded-uart-common
sps_common_dir := .

CFLAGS := -Wall -I. -I${sensirion_uart_dir} -I${sps_common_dir}

sensirion_uart_objects := sensirion_uart_implementation.o sensirion_shdlc.o
sps_common_objects := sps_git_version.o
sps_binaries := sps30_example_usage

all_objects := ${sensirion_uart_objects} ${sps_common_objects} ${sps_binaries} sps30.o

.PHONY: all

all: ${sps_binaries}

sps_git_version.o: ${sps_common_dir}/sps_git_version.c
	$(CC) $(CFLAGS) -c -o $@ $^
sensirion_uart_implementation.o: ${sensirion_uart_dir}/sensirion_uart_implementation.c
	$(CC) $(CFLAGS) -c -o $@ $^
sensirion_shdlc.o: ${sensirion_uart_dir}/sensirion_shdlc.c
	$(CC) $(CFLAGS) -c -o $@ $^

sps30.o: sensirion_shdlc.o ${sps_common_dir}/sps_git_version.c sps30.h sps30.c

sps30_example_usage: ${sensirion_uart_objects} ${sps_common_objects} sps30.o sps30_example_usage.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LOADLIBES) $(LDLIBS)

clean:
	$(RM) ${all_objects} ${sps_binaries}
