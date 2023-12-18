#include <iostream>
#include <thread>
#include <vector>
#include <cstring>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <unistd.h>
#include <arpa/inet.h>

constexpr int PORT = 9999;
constexpr int BUFFER_SIZE = 1024;

void processMessage(int clientSocket) {
    char buffer[1024];
    ssize_t bytesRead;

    int i = 0;
    int j = 0;
    bool rem = false;
    while (true) {
        memset(buffer, 0, sizeof(buffer));
        bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
        if(bytesRead <= 0){
            break;
        }
        for( ; i < BUFFER_SIZE && j < BUFFER_SIZE; ){
            if(buffer[j] == '\0'){
                i = j + 1;
                j = i;
                continue;
            }
            if(buffer[j] == '\n'){
                if(!rem){
                    printf("Received from client: %.*s", j - i + 1, buffer + i);
                }
                else{
                    printf("%.*s", j - i + 1, buffer + i);
                    rem = false;
                }
                i = j + 1;
                j = i;
            }
            else{
                j = j + 1;
            }
        }
        if(j > i){
            printf("Received from client: %.*s", j - i + 1, buffer + i);
            rem = true;
        }
        i = 0;
        j = 0;
    }

    std::cout << "Client disconnected\n";
    close(clientSocket);
}

int main() {
    while (true) {
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

        std::cout << "Waiting for client connections...\n";

        sockaddr_in clientAddr;
        socklen_t clientAddrLen = sizeof(clientAddr);
        int clientSocket = accept(serverSocket, reinterpret_cast<struct sockaddr*>(&clientAddr), &clientAddrLen);
        if (clientSocket == -1) {
            std::cerr << "Error accepting client connection\n";
            break;
        }
        processMessage(clientSocket);
        close(serverSocket);
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }


    return 0;
}
