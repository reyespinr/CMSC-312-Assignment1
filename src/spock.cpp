/*
** spock.cpp -- reads numbers from a message queue, sorts them, and prints out
*/
#include <signal.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <unistd.h>

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "message_buffer.h"

// Global message queue id so the signal handler can access it
int global_msqid = -1;

// Function to handle SIGINT (Ctrl+C)
auto handle_sigint(int sig) -> void
{
  if (global_msqid != -1) {
    // Cleanup message queue
    if (msgctl(global_msqid, IPC_RMID, NULL) == -1) {
      perror("msgctl");
    }
    std::cout << "spock: message queue cleaned up" << std::endl;
  }
  exit(0);
}

int main()
{
  // Set up signal handler
  signal(SIGINT, handle_sigint);

  MessageBuffer buf, ack;
  int msqid;
  key_t key;

  if ((key = ftok("../src/kirk.cpp", 'B')) == -1) {
    perror("ftok");
    exit(1);
  }

  if ((msqid = msgget(key, 0644 | IPC_CREAT)) == -1) {
    perror("msgget");
    exit(1);
  }

  std::cout << "spock: ready to receive numbers, captain." << std::endl;

  std::vector<int> numbers;

  while (true) {
    ssize_t recvSize = msgrcv(msqid, &buf, sizeof(buf.mtext), 0, 0);
    if (recvSize == -1) {
      std::cout << "spock: message queue removed, waiting for a new queue." << std::endl;
      usleep(500000);  // 0.5 seconds
      if ((msqid = msgget(key, 0644 | IPC_CREAT)) == -1) {
        perror("msgget");
        exit(1);
      }
      global_msqid = msqid;

      continue;  // Stay in the loop even if receiving fails.
    }

    std::string message(buf.mtext);
    if (message == "end") {
      std::sort(numbers.begin(), numbers.end());
      for (int num : numbers) {
        std::cout << num << ' ';
      }
      std::cout << "\nspock: finished sorting." << std::endl;
      numbers.clear();

      // Send acknowledgment to kirk
      ack.mtype = 2;  // Use a different mtype for acknowledgment
      strncpy(ack.mtext, "done", sizeof(ack.mtext) - 1);
      ack.mtext[sizeof(ack.mtext) - 1] = '\0';  // Ensure null-termination
      if (msgsnd(msqid, &ack, sizeof(ack.mtext), 0) == -1) {
        perror("msgsnd");
      }
    } else {
      numbers.push_back(std::stoi(message));
    }
  }

  return EXIT_SUCCESS;
}
