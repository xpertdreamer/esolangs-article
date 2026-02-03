cmake_minimum_required(VERSION 3.31)
project(Piet_interp C)

set(CMAKE_C_STANDARD 17)

find_package(PNG REQUIRED)

add_executable(Piet_interp main.c)

target_link_libraries(Piet_interp PRIVATE PNG::PNG)