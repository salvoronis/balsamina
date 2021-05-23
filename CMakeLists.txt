cmake_minimum_required(VERSION 3.19)
project(Balsamina C)

set(CMAKE_C_STANDARD 11)

include_directories(core/inc ui/inc)

add_executable(Balsamina core/inc/core.h core/src/server/server.c core/src/client/client.c ui/src/main.c)

target_link_libraries(Balsamina pthread)