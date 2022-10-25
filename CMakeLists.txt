cmake_minimum_required(VERSION 3.21)
project(mora)

set(CMAKE_CXX_STANDARD 14)

add_executable(mora main.cpp constants.h LPS.cpp LPS.h byte_utils.cpp byte_utils.h)
