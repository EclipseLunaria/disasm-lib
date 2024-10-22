cmake_minimum_required(VERSION 3.11)
project(DisasmLib C)

set(CMAKE_C_STANDARD 11)



include_directories(include)
include_directories(internal)

add_library(disasm_lib
	src/arm_decoder.c
	src/thumb_decoder.c
	src/constants.c
	src/shift_ops.c
	src/instruction_builder.c
)

target_include_directories(disasm_lib PUBLIC
	$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
	$<INSTALL_INTERFACE:include>
)

install(TARGETS disasm_lib
	ARCHIVE DESTINATION lib
	LIBRARY DESTINATION lib
	RUNTIME DESTINATION bin
)

install(DIRECTORY include/ DESTINATION include)


