cmake_minimum_required(VERSION 3.22)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(app STATIC
    src/app.cpp
    src/fdcan_driver.cpp
    src/neopixel.cpp
    ${CMAKE_SOURCE_DIR}/gn10-can/drivers/stm32_fdcan/driver_stm32_fdcan.cpp

)

target_include_directories(app PUBLIC
    include
    ${CMAKE_SOURCE_DIR}/gn10-can        

)

target_link_libraries(app PRIVATE stm32cubemx)
target_link_libraries(app PUBLIC gn10_can)
