#include <netinet/in.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <algorithm>
#include <cstring>
#include <iostream>
#include <vector>

const int SERVER_PORT = 2080;
const int BACKLOG = 10;

void sortAndSendBackNumbers(int clientSock)
{
  std::vector<int> numbers;
  char buffer[1024];

  while (true) {
    memset(buffer, 0, 1024);
    ssize_t bytesReceived = recv(clientSock, buffer, 1023, 0);
    if (bytesReceived <= 0) break;

    std::string message(buffer);
    if (message == "end\n") break;

    int number = std::stoi(message);
    numbers.push_back(number);
  }

  std::sort(numbers.begin(), numbers.end());

  for (int number : numbers) {
    std::string message = std::to_string(number) + "\n";
    send(clientSock, message.c_str(), message.size(), 0);
  }

  // Close the client socket
  close(clientSock);
}

void signalHandler(int signal)
{
  wait(NULL);  // Clean up child processes
}

int main()
{
  signal(SIGCHLD, signalHandler);  // To avoid zombie processes

  int serverSock = socket(AF_INET, SOCK_STREAM, 0);
  if (serverSock < 0) {
    perror("Socket creation failed");
    return 1;
  }

  sockaddr_in serverAddress;
  serverAddress.sin_family = AF_INET;
  serverAddress.sin_addr.s_addr = INADDR_ANY;
  serverAddress.sin_port = htons(SERVER_PORT);

  if (bind(serverSock, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) {
    perror("Bind failed");
    return 1;
  }

  if (listen(serverSock, BACKLOG) < 0) {
    perror("Listen failed");
    return 1;
  }

  while (true) {
    int clientSock = accept(serverSock, NULL, NULL);
    if (clientSock < 0) {
      perror("Accept failed");
      continue;
    }

    pid_t pid = fork();
    if (pid < 0) {
      perror("Fork failed");
      close(clientSock);
    } else if (pid == 0) {
      // Child process
      close(serverSock);
      sortAndSendBackNumbers(clientSock);
      exit(0);
    } else {
      // Parent process
      close(clientSock);
    }
  }

  return 0;
}
