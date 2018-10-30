cmake_minimum_required(VERSION 3.12)
project(nginxtry)

set(CMAKE_CXX_STANDARD 11)

add_executable(nginxtry main.cpp client.h client.cpp server.h server.cpp)