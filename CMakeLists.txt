cmake_minimum_required(VERSION 3.16)
project(CMSC-312-Assignment1)

set(CMAKE_CXX_STANDARD 17)

include_directories(include)

# Executables for Q1
add_executable(ProcessA src/process_a.cpp)
add_executable(ProcessB src/process_b.cpp)
add_executable(ProcessC src/process_c.cpp)

# Executables for Q2
add_executable(SeparateForks src/separate_forks.cpp)
add_executable(NestedForks src/nested_forks.cpp)

# Executables for Q3
add_executable(Spock src/spock.c)
add_executable(Kirk src/kirk.c)

# Executables for Q4
add_executable(NewSpock src/spock.cpp)
add_executable(NewKirk src/kirk.cpp)

# Executables for Q5
add_executable(Server src/server.cpp)
add_executable(Client src/client.cpp)