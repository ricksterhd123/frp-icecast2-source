cmake_minimum_required(VERSION 3.16)
project(frp-radio-source)

find_library(m m)
find_library(dl dl)
find_library(shout shout)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/src/sqlite
    ${CMAKE_CURRENT_SOURCE_DIR}/src/streamer
)

add_library(
    sqlite3
    SHARED
    ${CMAKE_CURRENT_SOURCE_DIR}/src/sqlite/sqlite3.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/streamer/streamer.cpp
)

add_executable(frp-radio-source ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp)
target_link_libraries(frp-radio-source PRIVATE sqlite3 shout pthread dl m)
