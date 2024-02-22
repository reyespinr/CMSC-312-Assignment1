/*
** kirk.cpp -- writes a sequence of random numbers to a message queue
*/
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

auto main() -> int
{
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
  if (msgsnd(msqid, &buf, 4, 0) == -1) {
    std::cerr << "msgsnd error" << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
