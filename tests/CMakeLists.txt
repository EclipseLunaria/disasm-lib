cmake_minimum_required(VERSION 3.11)
project(DisasmLibTests C)

set(CMAKE_C_STANDARD 11)

include_directories(include)

add_subdirectory(lib/cunit/CUnit)

add_executable(run_tests
	test_main.c
	macro_tests.c
	alu_decoder_tests.c
	builder_tests.c
	psr_tests.c
	selector_tests.c
	multiply_decoding_tests.c
	thumb_decoder_tests.c
	coprocessor_tests.c
	memory_decoding_tests.c

	../src/arm_decoder.c
	../src/thumb_decoder.c
	../src/constants.c
	../src/shift_ops.c
	../src/instruction_builder.c
)


target_link_libraries(run_tests
	cunit
)

