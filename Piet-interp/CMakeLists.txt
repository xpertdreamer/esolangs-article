cmake_minimum_required(VERSION 3.31)
project(Piet_interp C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

find_package(PNG REQUIRED)
find_package(ZLIB REQUIRED)

include_directories(${PNG_INCLUDE_DIRS} ${ZLIB_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(Piet_interp main.c
)

target_link_libraries(Piet_interp PRIVATE PNG::PNG)