cmake_minimum_required(VERSION 3.16)
project(CMSC-312-Assignment1)

set(CMAKE_CXX_STANDARD 17)

include_directories(include)

add_executable(ProcessA src/shm_server.c)
# add_executable(ProcessB src/shm_client.c)
# add_executable(ProcessC src/shm_client.c) # You might need to rename this if the source file for process C is different

target_link_libraries(ProcessA rt) # Link the realtime library, necessary for shared memory on some systems

