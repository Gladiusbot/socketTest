#include <iostream>
#include <thread>
#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <unistd.h>
#include <arpa/inet.h>

constexpr int PORT = 9999;
constexpr int THREAD_POOL_SIZE = 100;

std::mutex mtx;
std::condition_variable cv;
std::queue<int> clientSockets;

void processMessage(int clientSocket) {
    char buffer[1024];
    ssize_t bytesRead;

    while ((bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0)) > 0) {
        buffer[bytesRead] = '\0';
        std::cout << "Received from client: " << buffer << std::endl;
    }

    std::cout << "Client disconnected\n";
    close(clientSocket);
}

void worker() {
    while (true) {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [] { return !clientSockets.empty(); });

        int clientSocket = clientSockets.front();
        clientSockets.pop();
        lock.unlock();

        processMessage(clientSocket);
    }
}

int main() {
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

    if (listen(serverSocket, THREAD_POOL_SIZE) == -1) {
        std::cerr << "Error listening on server socket\n";
        close(serverSocket);
        return 1;
    }

    std::vector<std::thread> threads;
    for (int i = 0; i < THREAD_POOL_SIZE; ++i) {
        threads.emplace_back(worker);
    }

    std::cout << "Waiting for client connections...\n";

    while (true) {
        sockaddr_in clientAddr;
        socklen_t clientAddrLen = sizeof(clientAddr);
        int clientSocket = accept(serverSocket, reinterpret_cast<struct sockaddr*>(&clientAddr), &clientAddrLen);
        if (clientSocket == -1) {
            std::cerr << "Error accepting client connection\n";
            break;
        }

        std::unique_lock<std::mutex> lock(mtx);
        clientSockets.push(clientSocket);
        lock.unlock();
        cv.notify_one();
    }

    for (auto& thread : threads) {
        thread.join();
    }

    close(serverSocket);

    return 0;
}
