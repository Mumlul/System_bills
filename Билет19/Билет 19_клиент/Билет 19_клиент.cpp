#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>

#pragma comment(lib, "ws2_32.lib")

const int DEFAULT_PORT = 11111;
const int BUFFER_SIZE = 4096;

int main() {
    setlocale(LC_ALL, "Rus");
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "Ошибка WSAStartup" << std::endl;
        return 1;
    }

    SOCKET clientSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "Не удалось создать сокет: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 1;
    }

    std::string serverIP;
    std::cout << "Введите IP-адрес сервера (например, 192.168.1.2): ";
    std::getline(std::cin, serverIP);

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(DEFAULT_PORT);
    inet_pton(AF_INET, serverIP.c_str(), &serverAddr.sin_addr);

    std::string message;
    while (true) {
        std::cout << "Введите сообщение для отправки (или 'exit' для выхода): ";
        std::getline(std::cin, message);

        if (message == "exit") {
            break;
        }

        if (sendto(clientSocket, message.c_str(), message.length(), 0, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
            std::cerr << "Не удалось отправить сообщение: " << WSAGetLastError() << std::endl;
            continue;
        }

        std::cout << "Сообщение отправлено на сервер!" << std::endl;
    }

    closesocket(clientSocket);
    WSACleanup();
    return 0;
}