# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# initalize pico_sdk from installed location
# (note this can come from environment, CMake cache etc)
set(PICO_SDK_PATH "C:/Users/Windows 10/Documents/Pico/pico-sdk")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(la_ola_5 C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

# Add any PICO_CONFIG entries specified in the Advanced settings
add_compile_definitions(PARAM_ASSERTIONS_ENABLED_IRQ=1)
add_compile_definitions(PARAM_ASSERTIONS_ENABLED_PWM=1)
add_compile_definitions(PARAM_ASSERTIONS_ENABLED_TIME=1)

add_executable(la_ola_5 la_ola_5.c )

pico_set_program_name(la_ola_5 "la_ola_5")
pico_set_program_version(la_ola_5 "0.1")

pico_enable_stdio_uart(la_ola_5 1)
pico_enable_stdio_usb(la_ola_5 0)

# Add the standard library to the build
target_link_libraries(la_ola_5 pico_stdlib hardware_pwm)

pico_add_extra_outputs(la_ola_5)

