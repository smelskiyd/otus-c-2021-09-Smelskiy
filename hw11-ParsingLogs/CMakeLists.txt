project(OtusHW11_ParsingLogs C)

set(SRC
        FileList.h                  FileList.c
        CombinedLog.h               CombinedLog.c
        FileWithMutex.h             FileWithMutex.c
        LogsProcessor.h             LogsProcessor.c
        ParallelLogsProcessor.h     ParallelLogsProcessor.c)

add_executable(OtusHW11_ParsingLogs main.c ${SRC})
add_dependencies(OtusHW11_ParsingLogs MyHashMap)
target_link_libraries(OtusHW11_ParsingLogs PRIVATE -pthread MyHashMap)
