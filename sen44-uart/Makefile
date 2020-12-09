# See user_config.inc for build customizations
-include user_config.inc
include default_config.inc

.PHONY: all clean

all: sen44_example_usage

sen44_example_usage: clean
	$(CC) $(CFLAGS) -o $@ ${sen44_uart_sources} ${uart_sources} ${sen44_uart_dir}/sen44_example_usage.c

clean:
	$(RM) sen44_example_usage
