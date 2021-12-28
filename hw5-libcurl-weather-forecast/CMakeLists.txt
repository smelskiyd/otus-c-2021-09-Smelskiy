cmake_minimum_required(VERSION 3.20)
project(OtusHW5_WeatherForecast C)

add_subdirectory(3d-party)

add_executable(OtusHW5_WeatherForecast main.c)

target_compile_options(OtusHW5_WeatherForecast PRIVATE -Wall -Wextra -Wpedantic -std=c11)
target_link_libraries(OtusHW5_WeatherForecast PRIVATE -lcurl)

target_link_libraries(OtusHW5_WeatherForecast PUBLIC jansson)
