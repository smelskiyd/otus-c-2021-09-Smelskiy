project(OtusHW11_ParsingLogs C)

set(SRC
        FileList.h      FileList.c
        CombinedLog.h   CombinedLog.c
        LogsProcessor.h LogsProcessor.c
        FileWithMutex.h FileWithMutex.c)

add_executable(OtusHW11_ParsingLogs main.c ${SRC})
target_link_libraries(OtusHW11_ParsingLogs -pthread MyHashMap)
