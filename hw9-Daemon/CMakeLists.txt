project(OtusHW9_MyDaemon C)

set(SRC
        FileInfoMonitoring.h    FileInfoMonitoring.c)

add_executable(OtusHW9_MyDaemon MyDaemon.c ${SRC})
