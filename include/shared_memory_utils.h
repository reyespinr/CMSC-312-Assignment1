// shared_memory_utils.h
#ifndef SHARED_MEMORY_UTILS_H
#define SHARED_MEMORY_UTILS_H

#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>

#include <fstream>
#include <iostream>

bool attachToSharedMemory(int & shm_id, char *& ptr)
{
  ptr = static_cast<char *>(shmat(shm_id, nullptr, 0));
  return ptr != (char *)-1;  // Return true if attachment was successful
}

bool loadSharedMemoryIds(
  const char * filename, int & shm_id_pid, int & shm_id_proc_id, int & shm_id_str)
{
  std::ifstream ids_file(filename);
  if (!ids_file.is_open()) {
    std::cerr << "Waiting for shared memory IDs file to become available...\n";
    return false;
  }

  ids_file >> shm_id_pid >> shm_id_proc_id >> shm_id_str;
  bool result = ids_file.good();
  ids_file.close();
  return result;
}

#endif  // SHARED_MEMORY_UTILS_H
