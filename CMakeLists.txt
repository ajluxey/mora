cmake_minimum_required(VERSION 3.16)
project(mora_sponge)

set(CMAKE_CXX_STANDARD 14)

add_executable(mora_sponge main.cpp mora/constants.h mora/byte_utils.cpp mora/byte_utils.h mora/mora.cpp mora/mora.h mora/test_of_parts.cpp mora/test_of_parts.h sponge/sponge_construction.cpp sponge/sponge_construction.h sponge/sponge_crypto.cpp sponge/sponge_crypto.h)
