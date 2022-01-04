cmake_minimum_required(VERSION 3.15)
project(OtusHW C)

set(CMAKE_C_STANDARD 11)

add_compile_options(-Wall -Wextra -Wpedantic -std=c11)

add_subdirectory(hw1-rarjpeg)
add_subdirectory(hw2-utf-convertor)
add_subdirectory(hw3-hashmap)
add_subdirectory(hw5-libcurl-weather-forecast)
add_subdirectory(hw6-Logger)
add_subdirectory(hw7-Assembly)
