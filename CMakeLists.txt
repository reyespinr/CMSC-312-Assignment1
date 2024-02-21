cmake_minimum_required(VERSION 3.16)
project(CMSC-312-Assignment1)

set(CMAKE_CXX_STANDARD 17)

include_directories(include)

add_executable(ProcessA src/shm_server.cpp)
add_executable(ProcessB src/process_b.cpp)
