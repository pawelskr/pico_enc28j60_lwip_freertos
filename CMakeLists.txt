# Set minimum required version of CMake
cmake_minimum_required(VERSION 3.12)

# Include build functions from Pico SDK
include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)
# We also need PICO EXTRAS
include($ENV{PICO_EXTRA_SDK_PATH}/external/pico_extras_import.cmake)
# Set name of project (as PROJECT_NAME) and C/C%2B%2B standards
project(ethernet C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Creates a pico-sdk subdirectory in our project for the libraries
pico_sdk_init()

add_subdirectory(enc28j60)
add_subdirectory(hal)

# Tell CMake where to find the executable source file
add_executable(${PROJECT_NAME}
        main.cpp
        )
# Create map/bin/hex/uf2 files
pico_add_extra_outputs(${PROJECT_NAME})

# This is required to include lwipopts.h
target_include_directories(lwip INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}/app
        )

# Link to pico_stdlib (gpio, time, etc. functions)
target_link_libraries(${PROJECT_NAME} PUBLIC
        pico_stdlib hardware_spi hardware_irq hal enc28j60_driver lwip
        )

target_link_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/app)

# Enable usb output, disable uart output
pico_enable_stdio_usb(${PROJECT_NAME} 1)
pico_enable_stdio_uart(${PROJECT_NAME} 0)
