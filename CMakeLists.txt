cmake_minimum_required(VERSION 4.0)
project(COEN366_Server)

set(CMAKE_CXX_STANDARD 14)

add_executable(COEN366_Server main.cpp
        header/Server.h
        source/Server.cpp
        header/Globals.h
        header/UDP.h
        source/UDP.cpp
        header/HMAC.h
        source/HMAC.cpp
        header/Header.h
        source/Header.cpp
        header/Types.h)


target_link_libraries(COEN366_Server PRIVATE ws2_32)
