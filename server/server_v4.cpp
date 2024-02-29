#include <arpa/inet.h>
#include <unistd.h>

#include <condition_variable>
#include <cstring>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>
#define DEBUG

constexpr uint PORT = 9999;
// header:4bytes
constexpr uint HEADER_LENGTH = 4;
// body:1024bytes max
constexpr uint BODY_LENGTH = 1024;
// total buffer size
constexpr uint MAX_LENGTH = BODY_LENGTH + HEADER_LENGTH;
struct TlvData {
  int head;
  const char* body;
};

/**
 *  process according to TLV protocol
 */
void processMessage(int client_socket) {
  char buffer[MAX_LENGTH];
  uint bytes_read = 0;
  // length of data body
  uint data_length = 0;
  memset(buffer, 0, sizeof(buffer));

  while (true) {
    // read into buffer
    if (bytes_read != 0) {
      // bytes_read = bytes_read + recv(client_socket, buffer + bytes_read,
      // sizeof(buffer) - bytes_read, 0);
      bytes_read = recv(client_socket, buffer, sizeof(buffer), 0);
    } else {
      bytes_read = recv(client_socket, buffer, sizeof(buffer), 0);
    }

    // parse header
    if (bytes_read < HEADER_LENGTH) {
      continue;
    } else {
      std::memcpy(&data_length, buffer, sizeof(uint));
#ifdef DEBUG
      std::cout << "got patcket length:" << data_length << std::endl;
#endif
    }

    // invalid data length
    if (data_length > BODY_LENGTH) {
      std::cerr << "invalid data length" << std::endl;
      break;
    }

    // body transfer not finished yet
    if (data_length + HEADER_LENGTH > bytes_read) {
      continue;
    } else {  // body transfer finished, print body
      for (uint i = HEADER_LENGTH; i < HEADER_LENGTH + data_length; ++i) {
        std::cout << buffer[i];
      }
      std::cout << std::endl;
      // std::string resolved_string(buffer + HEADER_LENGTH, data_length);
      // std::cout << "resolved from packet:" << resolved_string << std::endl;
    }

    // after print, remove used message. Careful: do not delete next packet
    uint left_idx = 0;
    uint right_idx = data_length + HEADER_LENGTH;
    while (right_idx < bytes_read) {
      buffer[left_idx] = buffer[right_idx];
      left_idx++;
      right_idx++;
    }
    // memset(buffer + left_idx, 0, sizeof(buffer) - left_idx);
    bytes_read = left_idx;
  }

  std::cout << "Client disconnected\n";
  close(client_socket);
}

int main() {
  try {
    // create socket
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
      std::cerr << "Error creating server socket\n";
      return 1;
    }

    // bind
    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);
    if (bind(server_socket, reinterpret_cast<struct sockaddr*>(&server_addr),
             sizeof(server_addr)) == -1) {
      std::cerr << "Error binding server socket\n";
      close(server_socket);
      return 1;
    }

    while (true) {
      if (listen(server_socket, 1) == -1) {
        std::cerr << "Error listening on server socket\n";
        close(server_socket);
        return 1;
      }

      std::cout << "Waiting for client connections...\n";

      sockaddr_in client_addr;
      socklen_t client_addrLen = sizeof(client_addr);
      int client_socket = accept(
          server_socket, reinterpret_cast<struct sockaddr*>(&client_addr),
          &client_addrLen);
      if (client_socket == -1) {
        std::cerr << "Error accepting client connection\n";
        break;
      }
      processMessage(client_socket);
      close(server_socket);
      std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
  } catch (std::exception& e) {
    std::cerr << "Exception" << e.what() << std::endl;
  }

  return 0;
}
