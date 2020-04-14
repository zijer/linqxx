cmake_minimum_required(VERSION 3.9)
set(CMAKE_CXX_STANDARD 17)
project(linqxx VERSION 0.1)

include_directories(include/)
add_executable(linqxx source/test.cpp)