#include <csignal>
#include <cstring>
#include <fstream>
#include <iostream>

#include "shared_memory.h"

// Global instance for cleanup
SharedMemory * shm_pid = nullptr;
SharedMemory * shm_proc_id = nullptr;
SharedMemory * shm_str = nullptr;

// Signal handler for cleaning up shared memory
auto signalHandler(int signum) -> void
{
  std::cout << "Interrupt signal (" << signum << ") received.\n";
  delete shm_pid;
  delete shm_proc_id;
  delete shm_str;
  exit(signum);
}

auto main() -> int
{
  std::signal(SIGINT, signalHandler);
  std::ofstream ids_file("shm_ids.txt", std::ios::out | std::ios::trunc);

  try {
    shm_pid = new SharedMemory(IPC_PRIVATE, 1024);
    shm_proc_id = new SharedMemory(IPC_PRIVATE, 1024);
    shm_str = new SharedMemory(IPC_PRIVATE, 1024);

    if (!ids_file.is_open()) {
      throw std::runtime_error("Failed to open file for shared memory IDs.");
    }

    ids_file << shm_pid->getId() << "\n";
    ids_file << shm_proc_id->getId() << "\n";
    ids_file << shm_str->getId() << "\n";
    ids_file.close();

    sprintf(shm_pid->getPtr(), "%d", getpid());
    strcpy(shm_proc_id->getPtr(), "A");
    strcpy(shm_str->getPtr(), "I am Process A");

    std::cout << "I am Process A - " << shm_pid->getPtr() << std::endl;

    bool is_b_done = false;

    while (true) {
      if (strcmp(shm_proc_id->getPtr(), "B") == 0 && !is_b_done) {
        std::cout << shm_str->getPtr() << " - " << shm_pid->getPtr() << std::endl;
        is_b_done = true;
      } else if (strcmp(shm_proc_id->getPtr(), "C") == 0) {
        std::cout << shm_str->getPtr() << " - " << shm_pid->getPtr() << std::endl;
        break;
      }
      usleep(500000);  // 0.5 seconds
    }

    std::cout << "Goodbye " << getpid() << std::endl;
  } catch (const std::exception & e) {
    std::cerr << "An exception occurred: " << e.what() << std::endl;
    delete shm_pid;
    delete shm_proc_id;
    delete shm_str;
    return EXIT_FAILURE;
  }

  delete shm_pid;
  delete shm_proc_id;
  delete shm_str;
  return EXIT_SUCCESS;
}