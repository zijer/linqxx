cmake_minimum_required(VERSION 3.9)
project(linqxxdemo)
set(CMAKE_CXX_STANDARD 17)

find_package(linqxx CONFIG REQUIRED)

include_directories(/usr/local/include/linqxx)

add_executable(linqxxdemo main.cpp)
target_link_libraries(linqxxdemo linqxx)