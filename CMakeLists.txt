cmake_minimum_required(VERSION 3.24)

project(keyloggger)

set(SRC keylogger.c args.c kill.c start.c mapping.c)

set(CMAKE_C_COMPILER /usr/bin/gcc)

add_compile_options(-DNDBUG) # disable modules of debug

include_directories(${PROJECT_SOURCE_DIR})

add_executable(keyloggger ${SRC})

