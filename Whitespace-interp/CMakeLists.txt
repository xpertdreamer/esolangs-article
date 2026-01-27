cmake_minimum_required(VERSION 3.31)
project(Whitespace_interp LANGUAGES C)

set(CMAKE_C_STANDARD 17)

add_executable(Whitespace_interp main.c
        interpreter.h
        interpreter.c
        instruction.c
        m_interpreter.c
        instruction.h
        config.h)
