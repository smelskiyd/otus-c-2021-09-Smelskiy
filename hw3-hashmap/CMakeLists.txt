cmake_minimum_required(VERSION 3.15)
project(OtusHW3_HashMap C)

set(SRC
        Hash.h      Hash.c
        HashMap.h   HashMap.c)

add_library(MyHashMap STATIC ${SRC})
target_include_directories(MyHashMap PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(OtusHW3_HashMap main.c)
target_link_libraries(OtusHW3_HashMap PRIVATE MyHashMap)
