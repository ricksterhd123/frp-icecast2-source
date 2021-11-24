cmake_minimum_required(VERSION 3.16)
project(frp-radio-source)

find_library(m m)
find_library(dl dl)
find_library(pthread pthread)
find_library(shout shout)

include_directories(./sqlite)
link_directories(./sqlite)

add_executable(frp-radio-source main.c)
target_link_libraries(frp-radio-source PRIVATE shout pthread dl m)