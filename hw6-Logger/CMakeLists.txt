cmake_minimum_required(VERSION 3.20)
project(OtusHW6_Logger C)

set(SRC
        MyLogger.h    MyLogger.c)

add_library(MyLogger STATIC ${SRC})

add_executable(logger_tests main.c)
target_link_libraries(logger_tests PRIVATE MyLogger)
