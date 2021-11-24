cmake_minimum_required(VERSION 3.16)
project(frp-radio-source)

find_library(m m)
find_library(dl dl)
find_library(pthread pthread)
find_library(shout shout)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/sqlite)
add_library(sqlite3 SHARED ${CMAKE_CURRENT_SOURCE_DIR}/src/sqlite/sqlite3.c)

add_executable(frp-radio-source ${CMAKE_CURRENT_SOURCE_DIR}/src/main.c)
target_link_libraries(frp-radio-source PRIVATE sqlite3 shout pthread dl m)
