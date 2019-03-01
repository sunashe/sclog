cmake_minimum_required(VERSION 3.13)
project(sclog)

set(CMAKE_CXX_STANDARD 11)
include_directories(spdlog/include)

include_directories(./include)
add_executable(sclog main.cpp sclog.cc vs_log.cc)
add_subdirectory(strings)
target_link_libraries(sclog strings)

add_executable(test_spdlog test_spdlog.cc)