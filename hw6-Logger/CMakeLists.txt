cmake_minimum_required(VERSION 3.15)
project(OtusHW6_Logger C)

set(SRC
        MyLogger.h    MyLogger.c)

add_library(MyLogger STATIC ${SRC})

target_link_options(MyLogger PUBLIC -rdynamic)

set(MAX_HEADER_LOG_LENGTH 150)
set(MAX_BACKTRACE_SIZE 100)
target_compile_definitions(MyLogger PUBLIC -DMAX_HEADER_LOG_LENGTH=${MAX_HEADER_LOG_LENGTH})
target_compile_definitions(MyLogger PUBLIC -DMAX_BACKTRACE_SIZE=${MAX_BACKTRACE_SIZE})

add_executable(logger_example logger_example.c)
target_link_libraries(logger_example PRIVATE MyLogger)
