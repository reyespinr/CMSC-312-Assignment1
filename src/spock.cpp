/*
** spock.cpp -- reads numbers from a message queue, sorts them, and prints out
*/

#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "message_buffer.h"

auto main() -> int
try {
  MessageBuffer buf;
  int msqid;
  key_t key;

  if ((key = ftok("kirk.cpp", 'B')) == -1) {
    throw std::runtime_error("ftok error");
  }

  if ((msqid = msgget(key, 0644)) == -1) {
    throw std::runtime_error("msgget error");
  }

  std::cout << "spock: ready to receive numbers, captain." << std::endl;

  std::vector<int> numbers;

  while (true) {
    if (msgrcv(msqid, &buf, sizeof(buf.mtext), 0, 0) == -1) {
      throw std::runtime_error("msgrcv error");
    }
    std::string message(buf.mtext);
    if (message == "end") {
      std::sort(numbers.begin(), numbers.end());
      for (int num : numbers) {
        std::cout << num << ' ';
      }
      std::cout << "\nspock: ready for more numbers, captain." << std::endl;
      numbers.clear();
    } else {
      numbers.push_back(std::stoi(message));
    }
  }
} catch (const std::exception & e) {
  std::cerr << "Exception: " << e.what() << std::endl;
  return EXIT_FAILURE;
}
