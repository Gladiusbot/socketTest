#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>

constexpr int BYTE_LENGTH = 20;
constexpr int PORT = 9999;

int main() {
    // 创建 Socket 服务器
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        std::cerr << "Error creating server socket\n";
        return 1;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORT);

    if (bind(serverSocket, reinterpret_cast<struct sockaddr*>(&serverAddr), sizeof(serverAddr)) == -1) {
        std::cerr << "Error binding server socket\n";
        close(serverSocket);
        return 1;
    }

    if (listen(serverSocket, 1) == -1) {
        std::cerr << "Error listening on server socket\n";
        close(serverSocket);
        return 1;
    }

    std::cout << "Waiting for client connection...\n";

    // 获取客户端连接
    sockaddr_in clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);
    int clientSocket = accept(serverSocket, reinterpret_cast<struct sockaddr*>(&clientAddr), &clientAddrLen);
    if (clientSocket == -1) {
        std::cerr << "Error accepting client connection\n";
        close(serverSocket);
        return 1;
    }

    std::cout << "Client connected\n";

    // 循环获取客户端发送的信息
    while (true) {
        // 循环获取客户端发送的信息
        char bytes[BYTE_LENGTH];
        // 读取客户端发送的信息
        ssize_t count = recv(clientSocket, bytes, BYTE_LENGTH, 0);
        if (count > 0) {
            // 成功接收到有效消息并打印
            std::cout << "接收到客户端的信息是: " << bytes << std::endl;
        } else if (count == 0) {
            // 连接关闭
            std::cout << "客户端断开连接\n";
            break;
        } else {
            // 接收出错
            std::cerr << "Error receiving data from client\n";
            break;
        }
    }

    close(clientSocket);
    close(serverSocket);

    return 0;
}
