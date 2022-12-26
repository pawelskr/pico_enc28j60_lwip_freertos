# Set minimum required version of CMake
cmake_minimum_required(VERSION 3.12)

# Include build functions from Pico SDK
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/pico_sdk_import.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/FreeRTOS_Kernel_import.cmake)

project(ethernet C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Creates a pico-sdk subdirectory in our project for the libraries
pico_sdk_init()

add_subdirectory(enc28j60)
add_subdirectory(hal)
add_compile_definitions(ENC_DEBUG_OFF=1 NO_SYS=0)

add_executable(${PROJECT_NAME}
        main.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/app/enc_os_lwip_glue.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/app/hooks.c
        )

# Create map/bin/hex/uf2 files
pico_add_extra_outputs(${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} PUBLIC
        pico_stdlib hardware_spi hardware_irq hal enc28j60_driver pico_lwip pico_lwip_contrib_freertos
        FreeRTOS-Kernel-Heap4
        )

target_include_directories(${PROJECT_NAME} PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/app
        ${CMAKE_CURRENT_SOURCE_DIR}/FreeRTOS_port
        )

# disable usb output, disable uart output
pico_enable_stdio_usb(${PROJECT_NAME} 0)
pico_enable_stdio_uart(${PROJECT_NAME} 0)
