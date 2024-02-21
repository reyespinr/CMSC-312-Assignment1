#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>

#include "shared_memory_utils.h"

auto main() -> int
{
  int shm_id_pid = -1, shm_id_proc_id = -1, shm_id_str = -1;
  char *pid_ptr = nullptr, *proc_id_ptr = nullptr, *message_ptr = nullptr;

  while (!loadSharedMemoryIds("shm_ids.txt", shm_id_pid, shm_id_proc_id, shm_id_str) ||
         !attachToSharedMemory(shm_id_pid, pid_ptr) ||
         !attachToSharedMemory(shm_id_proc_id, proc_id_ptr) ||
         !attachToSharedMemory(shm_id_str, message_ptr)) {
    usleep(1000000);  // Wait for 1 second before trying again
    // std::cerr << "Retrying to attach to shared memory...\n";
  }

  // Once attached, proceed with the rest of the process logic
  // std::cout << "Attached to shared memory successfully.\n";

  // Wait for Process B to complete
  while (strcmp(proc_id_ptr, "B") != 0) {
    usleep(500000);  // 0.5 seconds
  }
  //Extra delay to make sure A has time to print out and acknowledge B
  usleep(500000);  // 0.5 seconds

  // Write PID to the first shared memory segment
  sprintf(pid_ptr, "%d", getpid());

  // Signal completion to Process A
  strcpy(proc_id_ptr, "C");

  // Write message to the third shared memory segment
  strcpy(message_ptr, "I am Process C");

  // Detach from shared memory
  shmdt(pid_ptr);
  shmdt(proc_id_ptr);
  shmdt(message_ptr);

  return EXIT_SUCCESS;
}
