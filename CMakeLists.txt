cmake_minimum_required(VERSION 3.16)
project(CMSC-312-Assignment1)

set(CMAKE_CXX_STANDARD 17)

include_directories(include)

add_executable(ProcessA src/process_a.cpp)
add_executable(ProcessB src/process_b.cpp)
add_executable(ProcessC src/process_c.cpp)

add_executable(SeparateForks src/separate_forks.cpp)
add_executable(NestedForks src/nested_forks.cpp)