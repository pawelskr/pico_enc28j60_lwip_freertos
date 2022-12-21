set(NAME enc28j60_driver)

add_library(${NAME} ${CMAKE_CURRENT_SOURCE_DIR}/src/enc28j60.cpp)
target_include_directories(${NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(${NAME} PRIVATE hal)