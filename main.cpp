#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>

constexpr int BYTE_LENGTH = 20;
constexpr int PORT = 9999;
const char* SERVER_IP = "39.104.209.73";

int main() {
    // 创建 Socket 客户端并尝试连接服务器端
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
        std::cerr << "Error connecting to server\n";
        close(clientSocket);
        return 1;
    }

    // 发送的消息内容
    const char* message = "Hi, C++.";

    // 使用输出流发送消息
    try {
        for (int i = 0; i < 10; i++) {
            // 发送消息
            ssize_t sentBytes = send(clientSocket, message, strlen(message), 0);
            if (sentBytes == -1) {
                std::cerr << "Error sending message to server\n";
                break;
            }
        }
    } catch (...) {
        std::cerr << "Exception while sending message\n";
    }

    close(clientSocket);

    return 0;
}
