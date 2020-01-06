cmake_minimum_required(VERSION 3.15 FATAL_ERROR)
project(pps VERSION 0.1.0)

set(CMAKE_C_STANDARD 11)

set(SOURCE_FILES
    src/main.c
)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

    add_executable(lab12C src/main.c)