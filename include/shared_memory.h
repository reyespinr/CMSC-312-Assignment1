// shared_memory.h
#ifndef SHARED_MEMORY_H
#define SHARED_MEMORY_H

#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>

#include <stdexcept>

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

  auto cleanup() -> void
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

#endif  // SHARED_MEMORY_H
