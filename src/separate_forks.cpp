#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <unistd.h>

#include <iostream>

auto executeProcess(const char * processName) -> void
{
  const char * argv[] = {processName, nullptr};
  execvp(processName, const_cast<char * const *>(argv));
  // If execvp returns, there was an error
  std::cerr << "Failed to execute " << processName << std::endl;
  _exit(1);  // Use _exit in child
}

auto main() -> int
{
  // Fork Process B
  pid_t pidB = fork();
  if (pidB < 0) {
    std::cerr << "Failed to fork Process B" << std::endl;
    return -1;
  } else if (pidB == 0) {
    // Child process B
    executeProcess("./ProcessB");
  }

  // Fork Process C
  pid_t pidC = fork();
  if (pidC < 0) {
    std::cerr << "Failed to fork Process C" << std::endl;
    return -1;
  } else if (pidC == 0) {
    // Child process C
    executeProcess("./ProcessC");
  }

  // Parent process: Execute Process A
  executeProcess("./ProcessA");

  // Wait for both child processes to complete
  waitpid(pidB, NULL, 0);
  waitpid(pidC, NULL, 0);

  return 0;
}
