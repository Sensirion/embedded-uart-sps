include ../embedded-common/tests-common-config/base_config.inc
sps_driver_dir := ../
include ${sps_driver_dir}/sps30-uart/default_config.inc

sps30_test_binaries := sps30-test-uart

.PHONY: all clean prepare test

all: clean prepare test

prepare:
	cd ${sps_driver_dir} && $(MAKE) prepare

sps30-test-uart: sps30-uart-test.cpp ${sps30_uart_sources} ${uart_sources} ${sensirion_test_sources}
	$(CXX) ${TTYDEV} $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	$(RM) ${sps30_test_binaries}

test: prepare ${sps30_test_binaries}
	set -ex; for test in ${sps30_test_binaries}; do echo $${test}; ./$${test}; echo; done;
