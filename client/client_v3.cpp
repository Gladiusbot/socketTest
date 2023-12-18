#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>

constexpr int PORT = 9999;
constexpr int MESSAGE_COUNT = 1000;
constexpr const char* MESSAGE = "Hi, C++.";
const char* SERVER_IP = "39.104.209.73";

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

    if (connect(clientSocket, reinterpret_cast<struct sockaddr*>(&serverAddr), sizeof(serverAddr)) == -1) {
        std::cerr << "Error connecting to the server\n";
        close(clientSocket);
        return 1;
    }

    // 给服务器端发送 10 次消息
    for (int i = 0; i < MESSAGE_COUNT; ++i) {
        // 注意：结尾的 \n 不能省略，它表示按行写入
        std::string message = MESSAGE;
        message += "\n";

        ssize_t bytesSent = send(clientSocket, message.c_str(), message.size(), 0);
        if (bytesSent == -1) {
            std::cerr << "Error sending message to the server\n";
            break;
        }

    }

    close(clientSocket);

    return 0;
}
