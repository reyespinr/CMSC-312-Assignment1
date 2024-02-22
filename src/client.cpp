#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstdlib>
#include <ctime>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

const int PORT = 2080;
const char * SERVER_IP = "127.0.0.1";

std::vector<int> generateRandomNumbers(size_t count)
{
  std::vector<int> numbers;
  for (size_t i = 0; i < count; ++i) {
    numbers.push_back(rand() % 100);
  }
  return numbers;
}

std::string serializeNumbers(const std::vector<int> & numbers)
{
  std::stringstream ss;
  for (auto num : numbers) {
    ss << num << " ";
  }
  return ss.str();
}

int main()
{
  srand(time(nullptr));
  int sock = 0;
  struct sockaddr_in serv_addr;

  std::cout << "Client: Setting up socket..." << std::endl;
  if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    std::cerr << "Socket creation error\n";
    return -1;
  }

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(PORT);
  if (inet_pton(AF_INET, SERVER_IP, &serv_addr.sin_addr) <= 0) {
    std::cerr << "Invalid address/ Address not supported\n";
    return -1;
  }

  std::cout << "Client: Connecting to server..." << std::endl;
  if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
    std::cerr << "Connection Failed\n";
    return -1;
  }

  auto numbers = generateRandomNumbers(10);
  auto message = serializeNumbers(numbers);
  std::cout << "Client: Sending numbers to server: " << message << std::endl;
  send(sock, message.c_str(), message.length(), 0);

  std::cout << "Client: Waiting for server to sort numbers..." << std::endl;

  close(sock);
  return 0;
}
