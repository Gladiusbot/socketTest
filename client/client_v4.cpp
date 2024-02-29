#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>

#include <cstring>
#include <iostream>

#define DEBUG

constexpr int PORT = 9999;
constexpr int MESSAGE_COUNT = 1000;
constexpr const char* MESSAGE = "Hi, C++.";
const char* SERVER_IP = "39.104.209.73";

// TODO: add header
// TODO: generate random sized messages
int main() {
  int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
  if (clientSocket == -1) {
    std::cerr << "Error creating client socket\n";
    return 1;
  }

  sockaddr_in serverAddr;
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP);
  serverAddr.sin_port = htons(PORT);

  if (connect(clientSocket, reinterpret_cast<struct sockaddr*>(&serverAddr),
              sizeof(serverAddr)) == -1) {
    std::cerr << "Error connecting to the server\n";
    close(clientSocket);
    return 1;
  }

  // 给服务器端发送 10 次消息
  for (int i = 0; i < MESSAGE_COUNT; ++i) {
    std::string message = sizeof(MESSAGE) + MESSAGE;
#ifdef DEBUG
    std::cout << "sending:" << message << std::endl;
#endif
    ssize_t bytesSent = send(clientSocket, message.c_str(), message.size(), 0);
    if (bytesSent == -1) {
      std::cerr << "Error sending message to the server\n";
      break;
    }
  }

  close(clientSocket);

  return 0;
}
