#include <sys/shm.h>
#include <unistd.h>

#include <csignal>
#include <cstring>
#include <fstream>
#include <iostream>
#include <stdexcept>

class SharedMemory
{
public:
  SharedMemory(int shm_id)
  {
    // Attach to the existing shared memory segment
    ptr = static_cast<char *>(shmat(shm_id, nullptr, 0));
    if (ptr == reinterpret_cast<char *>(-1)) {
      throw std::runtime_error("Failed to attach shared memory.");
    }
  }

  ~SharedMemory()
  {
    // Detach from the shared memory segment
    if (ptr) {
      shmdt(ptr);
      ptr = nullptr;
    }
  }

  char * getPtr() const { return ptr; }

private:
  char * ptr = nullptr;
};

void signalHandler(int signum)
{
  std::cout << "Interrupt signal (" << signum << ") received, exiting.\n";
  exit(signum);
}

int main()
{
  // Setup signal handling
  std::signal(SIGINT, signalHandler);

  try {
    std::ifstream ids_file("shm_ids.txt");
    int shm_id_pid, shm_id_proc_id, shm_id_str;

    if (!ids_file.is_open()) {
      throw std::runtime_error("Failed to open file for shared memory IDs.");
    }

    ids_file >> shm_id_pid >> shm_id_proc_id >> shm_id_str;
    ids_file.close();

    SharedMemory shm_pid(shm_id_pid);
    SharedMemory shm_proc_id(shm_id_proc_id);
    SharedMemory shm_str(shm_id_str);

    // Write PID to the first shared memory segment
    sprintf(shm_pid.getPtr(), "%d", getpid());

    // Signal completion to Process A
    strcpy(shm_proc_id.getPtr(), "B");

    // Write message to the third shared memory segment
    strcpy(shm_str.getPtr(), "I am Process B");

    // Delay to allow Process A to react
    usleep(500000);  // Adjust this value as needed

  } catch (const std::exception & e) {
    std::cerr << "An exception occurred: " << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
