cmake_minimum_required(VERSION 3.16)
project(frp-radio-source)

find_library(m m)
find_library(dl dl)
find_library(shout shout)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/redis-plus-plus/)

add_executable(
    frp-radio-source
    ${CMAKE_CURRENT_SOURCE_DIR}/src/streamer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
)

target_link_libraries(frp-radio-source PRIVATE shout dl m redis++)
