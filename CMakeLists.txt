cmake_minimum_required(VERSION 3.21)
project(mora)

set(CMAKE_CXX_STANDARD 14)

add_executable(mora main.cpp mora/constants.h mora/byte_utils.cpp mora/byte_utils.h mora/mora.cpp mora/mora.h mora/test_of_parts.cpp mora/test_of_parts.h)
