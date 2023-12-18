#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>

constexpr int BYTE_LENGTH = 1024;
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

    char bytes[BYTE_LENGTH] = {0};

    int idx = 0;
    for(char& b: std::string(message)){
        bytes[idx] = b;
        idx++;
    }

    // 使用输出流发送消息
    try {
        for (int i = 0; i < 10; i++) {
            // 发送消息
            send(clientSocket, bytes, BYTE_LENGTH, 0);
        }
    } catch (...) {
        std::cerr << "Exception while sending message\n";
    }

    close(clientSocket);

    return 0;
}
