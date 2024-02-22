#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <algorithm>
#include <cstring>
#include <iostream>
#include <sstream>
#include <vector>

const int PORT = 2080;

void sortAndRespond(int new_socket)
{
  char buffer[1024] = {0};
  read(new_socket, buffer, 1024);
  std::cout << "Server: Received numbers from client: " << buffer << std::endl;

  std::stringstream ss(buffer);
  int n;
  std::vector<int> numbers;
  while (ss >> n) {
    numbers.push_back(n);
  }

  std::sort(numbers.begin(), numbers.end());
  std::cout << "Server: Sorted numbers are: ";
  for (auto num : numbers) {
    std::cout << num << " ";
  }
  std::cout << std::endl;

  // No need to send sorted numbers back to the client, just acknowledge
  const char * msg = "Numbers sorted and printed by server.";
  send(new_socket, msg, strlen(msg), 0);
  close(new_socket);
}

int main()
{
  int server_fd, new_socket;
  struct sockaddr_in address;
  int opt = 1;
  int addrlen = sizeof(address);

  std::cout << "Server: Setting up socket..." << std::endl;
  if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
    perror("socket failed");
    exit(EXIT_FAILURE);
  }

  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
    perror("setsockopt");
    exit(EXIT_FAILURE);
  }

  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(PORT);

  if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
    perror("bind failed");
    exit(EXIT_FAILURE);
  }

  if (listen(server_fd, 3) < 0) {
    perror("listen");
    exit(EXIT_FAILURE);
  }

  std::cout << "Server: Listening for connections..." << std::endl;
  while ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen))) {
    if (new_socket < 0) {
      perror("accept");
      exit(EXIT_FAILURE);
    }

    std::cout << "Server: Connection accepted. Forking for client..." << std::endl;
    int pid = fork();
    if (pid == 0) {  // Child process
      sortAndRespond(new_socket);
      exit(0);
    }
    // Parent process continues to listen for new connections
  }

  return 0;
}
