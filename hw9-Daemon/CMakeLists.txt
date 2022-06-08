project(OtusHW9_MyDaemon C)

set(MYDAEMON_SRC
        Daemonizer.h            Daemonizer.c
        FileInfoMonitoring.h    FileInfoMonitoring.c)

add_executable(OtusHW9_MyDaemon MyDaemon.c ${MYDAEMON_SRC})

add_executable(OtusHW9_TestClient TestClient.c)
add_executable(OtusHW9_Server Server.c)

include(InstallDaemon.cmake)
