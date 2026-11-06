cmake_minimum_required(VERSION 3.10)
project(brazo C)

set(CMAKE_C_STANDARD 99)

add_executable(brazo main.c)

target_link_libraries(brazo pigpio rt pthread)