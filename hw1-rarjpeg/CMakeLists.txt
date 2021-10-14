cmake_minimum_required(VERSION 3.20)
project(OtusHW1_Rarjpeg C)

set(CMAKE_C_STANDARD 11)

add_executable(OtusHW1_Rarjpeg main.c PKZip.h)

target_compile_options(OtusHW1_Rarjpeg PUBLIC -Wall -Wextra -Wpedantic -std=c11)
