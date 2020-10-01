test_common_dir := ../embedded-uart-common/test-config

sensirion_test_sources := ${test_common_dir}/sensirion_test_setup.h \
                          ${test_common_dir}/sensirion_test_setup.cpp

CXXFLAGS ?= $(CFLAGS) -fsanitize=address -I${test_common_dir}
LDFLAGS ?= -lasan -lstdc++ -lCppUTest -lCppUTestExt
