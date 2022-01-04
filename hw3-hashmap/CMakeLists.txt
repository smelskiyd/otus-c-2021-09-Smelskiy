cmake_minimum_required(VERSION 3.15)
project(OtusHW3_HashMap C)

set(SRC
        Hash.h      Hash.c
        HashMap.h   HashMap.c)

add_executable(OtusHW3_HashMap main.c ${SRC})
