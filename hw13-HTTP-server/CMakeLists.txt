project(OtusHW13_HTTPServer C)

set(UTILS_SRC
        Utils/HTTP.c    Utils/Utils.c)

set(SERVER_SRC
        Server/Server.c    Server/Worker.c)

add_executable(OtusHW13_HTTPServer main.c ${SERVER_SRC} ${UTILS_SRC})
target_link_libraries(OtusHW13_HTTPServer PRIVATE -pthread)
target_include_directories(OtusHW13_HTTPServer
        PRIVATE
            Utils
            Server)

add_executable(OtusHW13_Client Client.c ${UTILS_SRC})
target_include_directories(OtusHW13_Client
        PRIVATE Utils)
