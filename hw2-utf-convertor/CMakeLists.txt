cmake_minimum_required(VERSION 3.20)
project(OtusHW2_UTFEncoding C)

set(SRC
        koi8.h      koi8.c
        iso8859-5.h iso8859-5.c
        CP1251.h    CP1251.c
        utf8.h      utf8.c
        )

add_executable(OtusHW2_UTFEncoding main.c ${SRC})

target_compile_options(OtusHW2_UTFEncoding PUBLIC -Wall -Wextra -Wpedantic -std=c11)
