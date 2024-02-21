#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>

#include <csignal>
#include <cstring>
#include <fstream>
#include <iostream>

class SharedMemory
{
public:
  SharedMemory(key_t key, size_t size) : shm_id(-1), ptr(nullptr)
  {
    shm_id = shmget(key, size, IPC_CREAT | 0666);
    if (shm_id < 0) {
      throw std::runtime_error("Failed to get shared memory.");
    }
    ptr = static_cast<char *>(shmat(shm_id, nullptr, 0));
    if (ptr == reinterpret_cast<char *>(-1)) {
      throw std::runtime_error("Failed to attach shared memory.");
    }
  }

  ~SharedMemory() { cleanup(); }

  void cleanup()
  {
    if (ptr) {
      shmdt(ptr);
      ptr = nullptr;
    }
    if (shm_id >= 0) {
      shmctl(shm_id, IPC_RMID, nullptr);
      shm_id = -1;
    }
  }

  char * getPtr() const { return ptr; }
  int getId() const { return shm_id; }

private:
  int shm_id;
  char * ptr;
};

// Global instance for cleanup
SharedMemory * shm_pid = nullptr;
SharedMemory * shm_proc_id = nullptr;
SharedMemory * shm_str = nullptr;

// Signal handler for cleaning up shared memory
auto signalHandler(int signum) -> void
{
  std::cout << "Interrupt signal (" << signum << ") received.\n";

  // Cleanup
  if (shm_pid) {
    shm_pid->cleanup();
    delete shm_pid;
  }
  if (shm_proc_id) {
    shm_proc_id->cleanup();
    delete shm_proc_id;
  }
  if (shm_str) {
    shm_str->cleanup();
    delete shm_str;
  }

  // Terminate program
  exit(signum);
}

auto main() -> int
{
  // Register signal SIGINT and signal handler
  std::signal(SIGINT, signalHandler);

  std::ofstream ids_file("shm_ids.txt", std::ios::out | std::ios::trunc);

  try {
    shm_pid = new SharedMemory(IPC_PRIVATE, 1024);
    shm_proc_id = new SharedMemory(IPC_PRIVATE, 1024);
    shm_str = new SharedMemory(IPC_PRIVATE, 1024);

    if (!ids_file.is_open()) {
      throw std::runtime_error("Failed to open file for shared memory IDs.");
    }

    // Write the shared memory IDs to a file
    ids_file << shm_pid->getId() << std::endl;
    ids_file << shm_proc_id->getId() << std::endl;
    ids_file << shm_str->getId() << std::endl;
    ids_file.close();

    // Initialize shared memory
    sprintf(shm_pid->getPtr(), "%d", getpid());
    strcpy(shm_proc_id->getPtr(), "A");
    strcpy(shm_str->getPtr(), "I am Process A");

    // Output initial state
    std::cout << "I am Process A - " << shm_pid->getPtr() << std::endl;

    // Polling loop
    while (true) {
      if (strcmp(shm_proc_id->getPtr(), "B") == 0) {
        std::cout << shm_str->getPtr() << std::endl;
        strcpy(shm_proc_id->getPtr(), "A");
        sleep(1);
      } else if (strcmp(shm_proc_id->getPtr(), "C") == 0) {
        std::cout << shm_str->getPtr() << std::endl;
        break;
      }
      usleep(100000);
    }

    std::cout << "GoodBye " << shm_pid->getPtr() << std::endl;

  } catch (const std::exception & e) {
    std::cerr << "An exception occurred: " << e.what() << std::endl;
    // Perform cleanup in case of exception
    if (shm_pid) delete shm_pid;
    if (shm_proc_id) delete shm_proc_id;
    if (shm_str) delete shm_str;
    return EXIT_FAILURE;
  }

  // Perform cleanup before successful exit
  delete shm_pid;
  delete shm_proc_id;
  delete shm_str;
  return EXIT_SUCCESS;
}
