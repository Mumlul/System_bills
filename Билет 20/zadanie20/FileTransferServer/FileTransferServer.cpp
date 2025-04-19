#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <fstream>
#include <string>
#include <sys/stat.h> 
#include <direct.h> 

#pragma comment(lib, "ws2_32.lib")

const int BUFFER_SIZE = 4096;
const int DEFAULT_PORT = 11111;


bool CreateDirectoryRecursive(const std::string& path) {
    size_t pos = 0;
    std::string dir;
    int mdret;

    if (path[path.size() - 1] != '\\') {
        
        dir = path + "\\";
    }
    else {
        dir = path;
    }

    while ((pos = dir.find_first_of("\\", pos + 1)) != std::string::npos) {
        std::string subdir = dir.substr(0, pos);
        mdret = _mkdir(subdir.c_str());
        if (mdret != 0 && errno != EEXIST) {
            return false;
        }
    }

    return true;
}

bool ReceiveFile(SOCKET clientSocket, const std::string& filePath) {
   
    int64_t fileSize;
    int bytesReceived = recv(clientSocket, (char*)&fileSize, sizeof(fileSize), 0);
    if (bytesReceived != sizeof(fileSize)) {
        std::cerr << "не удалось получить размер файла" << std::endl;
        return false;
    }

   
    size_t lastSlash = filePath.find_last_of("\\");
    if (lastSlash != std::string::npos) {
        std::string dirPath = filePath.substr(0, lastSlash);
        if (!CreateDirectoryRecursive(dirPath)) {
            std::cerr << "не удалось создать директорию: " << dirPath << std::endl;
        }
    }

    std::cout << "получение файла: " << filePath << " (" << fileSize << " bytes)" << std::endl;

   
    std::ofstream file(filePath, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "не удалось открыть файл для записи: " << filePath << std::endl;
        return false;
    }

    char buffer[BUFFER_SIZE];
    int64_t totalReceived = 0;

    
    while (totalReceived < fileSize) {
        int bytesToReceive = (fileSize - totalReceived) > BUFFER_SIZE ? BUFFER_SIZE : (fileSize - totalReceived);
        bytesReceived = recv(clientSocket, buffer, bytesToReceive, 0);

        if (bytesReceived <= 0) {
            std::cerr << "не удалось получить данные файла" << std::endl;
            file.close();
            return false;
        }

        file.write(buffer, bytesReceived);
        totalReceived += bytesReceived;
    }

    file.close();
    std::cout << "файл успешно получен!" << std::endl;
    return true;
}

bool SendFile(SOCKET clientSocket, const std::string& filePath) {
   
    std::ifstream test(filePath, std::ios::binary);
    if (!test) {
        std::cerr << "файл не найден: " << filePath << std::endl;
        test.close();
        return false;
    }
    test.close();

   
    std::ifstream file(filePath, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        std::cerr << "не удалось открыть файл для чтения: " << filePath << std::endl;
        return false;
    }

   
    int64_t fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    std::cout << "отправка файла: " << filePath << " (" << fileSize << " байт)" << std::endl;

   
    if (send(clientSocket, (char*)&fileSize, sizeof(fileSize), 0) != sizeof(fileSize)) {
        std::cerr << "не удалось отправить размер файла" << std::endl;
        file.close();
        return false;
    }

    char buffer[BUFFER_SIZE];
    int64_t totalSent = 0;

    
    while (totalSent < fileSize) {
        file.read(buffer, BUFFER_SIZE);
        int bytesRead = file.gcount();
        int bytesSent = send(clientSocket, buffer, bytesRead, 0);

        if (bytesSent <= 0) {
            std::cerr << "не удалось отправить данные файла" << std::endl;
            file.close();
            return false;
        }

        totalSent += bytesSent;
    }

    file.close();
    std::cout << "файл успешно отправлен" << std::endl;
    return true;
}

void HandleClient(SOCKET clientSocket) {
    char command;
    char filename[256];
    int bytesReceived;

   
    bytesReceived = recv(clientSocket, &command, sizeof(command), 0);
    if (bytesReceived <= 0) {
        std::cout << "клиент отключился" << std::endl;
        closesocket(clientSocket);
        return;
    }

   
    bytesReceived = recv(clientSocket, filename, sizeof(filename), 0);
    if (bytesReceived <= 0) {
        std::cout << "ошибка получения названия файла" << std::endl;
        closesocket(clientSocket);
        return;
    }
    filename[bytesReceived] = '\0';

    std::string filePath(filename);

    switch (command) {
    case 'U':
        if (!ReceiveFile(clientSocket, filePath)) {
            std::cerr << "ошибка отправки файла" << std::endl;
        }
        break;

    case 'D': 
        if (!SendFile(clientSocket, filePath)) {
            std::cerr << "ошибка загрузки файла" << std::endl;
        }
        break;

    default:
        std::cerr << "неизвестная комманда: " << command << std::endl;
        break;
    }

   
    std::cout << "Соединение закрыто..." << std::endl;
    closesocket(clientSocket);
}

int main() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "ошибка WSAStartup" << std::endl;
        return 1;
    }

    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serverSocket == INVALID_SOCKET) {
        std::cerr << "не удалось создать сокет: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 1;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(DEFAULT_PORT);

    if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "ошибка Bind: " << WSAGetLastError() << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "ошибка Listen: " << WSAGetLastError() << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "сервер запущен, порт: " << DEFAULT_PORT << std::endl;
    std::cout << "ожидание подключения..." << std::endl;

    while (true) {
        SOCKET clientSocket = accept(serverSocket, NULL, NULL);
        if (clientSocket == INVALID_SOCKET) {
            std::cerr << "ошибка принятия: " << WSAGetLastError() << std::endl;
            continue;
        }

        std::cout << "клиент подключен" << std::endl;
        HandleClient(clientSocket);
    }

    closesocket(serverSocket);
    WSACleanup();
    return 0;
}