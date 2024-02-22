#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstring>
#include <iostream>
#include <string>
#include <vector>

const char * SERVER_IP = "127.0.0.1";
const int SERVER_PORT = 2080;

std::vector<int> generateRandomNumbers(size_t count)
{
  std::vector<int> numbers;
  for (size_t i = 0; i < count; ++i) {
    numbers.push_back(rand() % 100);  // Generate numbers between 0 and 99
  }
  return numbers;
}

void sendNumbers(int sockfd, const std::vector<int> & numbers)
{
  for (int number : numbers) {
    std::string message = std::to_string(number) + "\n";
    send(sockfd, message.c_str(), message.size(), 0);
  }
  // Indicate the end of transmission
  std::string endMessage = "end\n";
  send(sockfd, endMessage.c_str(), endMessage.size(), 0);
}

int main()
{
  srand(time(nullptr));  // Seed the random number generator

  int sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock < 0) {
    perror("Socket creation failed");
    return 1;
  }

  sockaddr_in serverAddress;
  serverAddress.sin_family = AF_INET;
  serverAddress.sin_port = htons(SERVER_PORT);
  inet_pton(AF_INET, SERVER_IP, &serverAddress.sin_addr);

  if (connect(sock, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) {
    perror("Connection failed");
    return 1;
  }

  std::vector<int> numbers = generateRandomNumbers(10);  // Generate 10 random numbers
  sendNumbers(sock, numbers);

  // Close the socket
  close(sock);

  return 0;
}
