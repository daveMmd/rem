cmake_minimum_required(VERSION 3.8)
project(tem_clion)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp SSA.cpp common.cpp)
add_executable(tem_clion ${SOURCE_FILES})