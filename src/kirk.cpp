/*
** kirk.cpp -- writes a sequence of random numbers to a message queue
*/
#include <signal.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>

#include <cstdlib>
#include <cstring>
#include <ctime>
#include <iostream>
#include <random>
#include <string>

#include "message_buffer.h"

constexpr int NUM_COUNT = 50;
constexpr int MAX_NUMBER = 100;

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
    std::cout << "kirk: message queue cleaned up" << std::endl;
  }
  exit(0);
}
auto main() -> int
{
  // Set up signal handler
  signal(SIGINT, handle_sigint);

  MessageBuffer buf;
  int msqid;
  key_t key;

  if ((key = ftok("kirk.cpp", 'B')) == -1) {
    std::cerr << "ftok error" << std::endl;
    return EXIT_FAILURE;
  }

  if ((msqid = msgget(key, 0644 | IPC_CREAT)) == -1) {
    std::cerr << "msgget error" << std::endl;
    return EXIT_FAILURE;
  }
  global_msqid = msqid;
  buf.mtype = 1;  // Set message type to 1 for all messages

  // Modern C++ random number generation
  std::mt19937 rng(time(nullptr));
  std::uniform_int_distribution<int> dist(0, MAX_NUMBER - 1);

  // Generate and send a sequence of random numbers
  for (int i = 0; i < NUM_COUNT; ++i) {
    int number = dist(rng);
    auto numStr = std::to_string(number);
    strncpy(buf.mtext, numStr.c_str(), sizeof(buf.mtext) - 1);
    buf.mtext[sizeof(buf.mtext) - 1] = '\0';

    if (msgsnd(msqid, &buf, numStr.size() + 1, 0) == -1) {
      std::cerr << "msgsnd error" << std::endl;
      return EXIT_FAILURE;
    }
  }
  // Send a termination message
  strncpy(buf.mtext, "end", sizeof(buf.mtext) - 1);
  buf.mtext[sizeof(buf.mtext) - 1] = '\0';
  if (msgsnd(msqid, &buf, strlen(buf.mtext) + 1, 0) == -1) {
    perror("msgsnd");
    exit(1);
  }

  // Wait for acknowledgment from spock
  MessageBuffer ack;
  if (msgrcv(msqid, &ack, sizeof(ack.mtext), 2, 0) == -1) {
    perror("msgrcv");
    exit(1);
  }

  // Check if the acknowledgment message is "done"
  if (strncmp(ack.mtext, "done", 4) == 0) {
    // Cleanup message queue
    if (msgctl(msqid, IPC_RMID, NULL) == -1) {
      perror("msgctl");
      exit(1);
    }
    std::cout << "kirk: message queue cleaned up" << std::endl;
  }

  return EXIT_SUCCESS;
}
