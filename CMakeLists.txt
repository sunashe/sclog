cmake_minimum_required(VERSION 2.8)
project(sclog)

set(CMAKE_CXX_STANDARD 11)
include_directories(spdlog/include)

include_directories(./include)
add_executable(test_sclog test_sclog.cpp vs_log.cc)
add_subdirectory(strings)
target_link_libraries(test_sclog strings)

add_executable(test_spdlog test_spdlog.cc)