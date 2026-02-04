cmake_minimum_required(VERSION 3.31)
project(Piet_interp C)

set(CMAKE_C_STANDARD 17)

find_package(PNG REQUIRED)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${PNG_INCLUDE_DIRS})

# Source files
set(SOURCES
        piet_cell.c
        piet_color.c
        piet_stack.c
        piet_interpreter.c
        piet_io.c
        main.c
)

# Header files
set(HEADERS
        piet_common.h
        piet_cell.h
        piet_color.h
        piet_stack.h
        piet_interpreter.h
        piet_io.h
)

add_executable(Piet_interp ${SOURCES} ${HEADERS})

target_link_libraries(Piet_interp PRIVATE PNG::PNG)