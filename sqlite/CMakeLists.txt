cmake_minimum_required(VERSION 3.16)
project(sqlite3)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
add_library(sqlite3 SHARED sqlite3.c)
