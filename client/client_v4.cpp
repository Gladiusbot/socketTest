#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>

#include <cstring>
#include <iostream>
#include <memory>
#include <random>

#define DEBUG

constexpr int PORT = 9999;
constexpr int HEADER_SIZE = 4;
constexpr int MESSAGE_COUNT = 500;
// const char* SERVER_IP = "39.104.209.73";
const char* SERVER_IP = "127.0.0.1";

struct TlvData {
  int head;
  const char* body;
};

std::string generate_rng_str() {
  int len = std::rand() % 10 + 1;
  std::string res = "";
  for (int i = 0; i < len; ++i) {
    res += 'a' + std::rand() % 26;
  }
  return res;
}

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
    std::string rng_str = generate_rng_str();
    std::string MESSAGE = "asdf" + rng_str;
    std::unique_ptr<TlvData> payload = std::make_unique<TlvData>();
    uint message_length = MESSAGE.length();
    payload->head = message_length;
    payload->body = MESSAGE.c_str();
    char* buffer = new char[4 + message_length];
    std::memcpy(buffer, payload.get(), sizeof(payload->head));
    std::memcpy(buffer + sizeof(payload->head), payload->body, message_length);
#ifdef DEBUG
    std::cout << "message_length:" << payload->head << std::endl;
    std::cout.write(buffer, 4 + message_length);
    std::cout << std::endl;
#endif
    ssize_t bytesSent =
        send(clientSocket, buffer, sizeof(payload->head) + message_length, 0);
    delete buffer;
    if (bytesSent == -1) {
      std::cerr << "Error sending message to the server\n";
      break;
    }
  }

  close(clientSocket);

  return 0;
}
