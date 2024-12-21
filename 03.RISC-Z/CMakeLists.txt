cmake_minimum_required(VERSION 3.25)
project(RISC-Z)

# set(CMAKE_GENERATOR "Unix Makefiles")
# set(CMAKE_GENERATOR "Ninja")

set(CMAKE_DEBUG_POSTFIX d)

if(CMAKE_BUILD_TYPE MATCHES Debug)
    message("debug mode")
    add_compile_definitions(DEBUG)
endif(CMAKE_BUILD_TYPE MATCHES Debug)

if(CMAKE_BUILD_TYPE MATCHES Release)
    message("release mode")
    add_compile_definitions(NDEBUG)
endif(CMAKE_BUILD_TYPE MATCHES Release)

set(SRC_LIST main.c cpu.c memory.c)

add_executable(risc-z ${SRC_LIST})

add_custom_command(
    OUTPUT fib.bin fib.elf fib.o
    COMMAND riscv64-unknown-elf-as ${CMAKE_SOURCE_DIR}/fib.s -o fib.o
    COMMAND riscv64-unknown-elf-ld fib.o -o fib.elf
    COMMAND riscv64-unknown-elf-objcopy -O binary fib.elf fib.bin
    DEPENDS ${CMAKE_SOURCE_DIR}/fib.s
    VERBATIM
)

add_custom_target(fib DEPENDS fib.bin)

enable_testing()

set(TEST_SRC_LIST test_cpu.c cpu.c memory.c)
add_executable(test_cpu ${TEST_SRC_LIST})
add_test(NAME CpuTests COMMAND test_cpu)

set(TEST_MAIN_SRC_LIST test_main.c)
add_executable(test_main ${TEST_MAIN_SRC_LIST})
add_dependencies(test_main risc-z fib)
add_test(NAME IntegrationTests COMMAND test_main)
set_tests_properties(IntegrationTests PROPERTIES
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)
