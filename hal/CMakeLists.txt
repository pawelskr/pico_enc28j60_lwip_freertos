set(NAME hal)

add_library(${NAME}
        ${CMAKE_CURRENT_SOURCE_DIR}/src/utils.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/spi.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/gpio_wrapper.cpp
        )
target_include_directories(${NAME} PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(${NAME} PRIVATE hardware_irq hardware_spi pico_stdlib)
