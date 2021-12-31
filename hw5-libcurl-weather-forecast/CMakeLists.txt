cmake_minimum_required(VERSION 3.20)
project(OtusHW5_WeatherForecast C)

add_subdirectory(3d-party)

set(SRC
        ReadURLData.h       ReadURLData.c
        JSONVisualizer.h    JSONVisualizer.c)

add_executable(OtusHW5_WeatherForecast main.c ${SRC})

target_link_libraries(OtusHW5_WeatherForecast PRIVATE -lcurl)

target_include_directories(OtusHW5_WeatherForecast PRIVATE ${JANSSON_INCLUDE_DIRS})
target_link_libraries(OtusHW5_WeatherForecast PRIVATE jansson)
