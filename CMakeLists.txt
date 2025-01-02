cmake_minimum_required(VERSION 3.29)
project(pc C)

set(CMAKE_C_STANDARD 99)

add_executable(pc main.c
        lexer.c
        lexer.h
        parser.c
        parser.h
        evaluator.c
        evaluator.h
        draw_utils.c
        draw_utils.h
        err.c
        err.h
        limits.c
        limits.h
)
