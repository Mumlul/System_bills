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


bool CreateDirectoryRecursive(const std::string& path) 
{
    size_t pos = 0;
    std::string dir;

    int mdret;

    if (path[path.size() - 1] != '\\') {
        

        dir = path + "\\";
    }
    else {
        dir = path;
    }

    while ((pos = dir.find_first_of("\\", pos + 1)) != std::string::npos) 
    {
        std::string subdir = dir.substr(0, pos);

        mdret = _mkdir(subdir.c_str());

        if (mdret != 0 && errno != EEXIST) 
        {
            return false;
        }
    }

    return true;
}

bool SendFile(SOCKET serverSocket, const std::string& filePath) 
{
  
    std::ifstream test(filePath, std::ios::binary);
    if (!test) 
    {
        std::cerr << "файл не найден: " << filePath << std::endl;
        test.close();

        return false;
    }
    test.close();

    
    std::ifstream file(filePath, std::ios::binary | std::ios::ate);
    if (!file.is_open())
    {

        std::cerr << "не удалось открыть файл для чтения: " << filePath << std::endl;

        return false;
    }

    
    int64_t fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    std::cout << "отправка файла: " << filePath << " (" << fileSize << " байт)" << std::endl;

   
    if (send(serverSocket, (char*)&fileSize, sizeof(fileSize), 0) != sizeof(fileSize)) 
    {
        std::cerr << "не удалось отправить размер файла" << std::endl;


        file.close();
        return false;
    }

    char buffer[BUFFER_SIZE];
    int64_t totalSent = 0;

   
    while (totalSent < fileSize)
    {
        file.read(buffer, BUFFER_SIZE);

        int bytesRead = file.gcount();



        int bytesSent = send(serverSocket, buffer, bytesRead, 0);

        if (bytesSent <= 0) 
        {
            std::cerr << "ошибка отправки данных файла" << std::endl;

            file.close();
            return false;
        }

        totalSent += bytesSent;
    }

    file.close();
    std::cout << "файл успешно отправлен!" << std::endl;
    return true;
}

bool ReceiveFile(SOCKET serverSocket, const std::string& filePath) {
   
    int64_t fileSize;
    int bytesReceived = recv(serverSocket, (char*)&fileSize, sizeof(fileSize), 0);

    if (bytesReceived != sizeof(fileSize))
    {
        std::cerr << "не удалось получить размер файла" << std::endl;
        return false;
    }

   
    size_t lastSlash = filePath.find_last_of("\\");
    if (lastSlash != std::string::npos) 
    {
        std::string dirPath = filePath.substr(0, lastSlash);
        if (!CreateDirectoryRecursive(dirPath)) 
        {

            std::cerr << "не удалось создать директорию: " << dirPath << std::endl;
        }
    }

    std::cout << "получение файла: " << filePath << " (" << fileSize << " байт)" << std::endl;

   
    std::ofstream file(filePath, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "не удалось открыть файл для записи: " << filePath << std::endl;
        return false;
    }

    char buffer[BUFFER_SIZE];
    int64_t totalReceived = 0;

 
    while (totalReceived < fileSize) {
        int bytesToReceive = (fileSize - totalReceived) > BUFFER_SIZE ? BUFFER_SIZE : (fileSize - totalReceived);
        bytesReceived = recv(serverSocket, buffer, bytesToReceive, 0);

        if (bytesReceived <= 0) {
            std::cerr << "ошибка получения данных файла" << std::endl;
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

int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::cout << "пример: " << argv[0] << " <server_ip> <command> <filename>" << std::endl;
        std::cout << "комманды:" << std::endl;
        std::cout << "  upload - отправить файл на сервер" << std::endl;
        std::cout << "  download - скачать файл с сервера" << std::endl;
        return 1;
    }

    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "ошибка WSAStartup" << std::endl;
        return 1;
    }

    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serverSocket == INVALID_SOCKET) {
        std::cerr << "ошибка создания сокета: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 1;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(DEFAULT_PORT);
    inet_pton(AF_INET, argv[1], &serverAddr.sin_addr);

    if (connect(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "ошибка подключения: " << WSAGetLastError() << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "подключено к серверу" << std::endl;

    
    char command;
    std::string cmd(argv[2]);
    if (cmd == "upload") {
        command = 'U';
    }
    else if (cmd == "download") {
        command = 'D';
    }
    else {
        std::cerr << "неизвестная команда: " << cmd << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

   
    if (send(serverSocket, &command, sizeof(command), 0) != sizeof(command)) {
        std::cerr << "не удалось отправить команду" << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    
    std::string filename(argv[3]);
    if (send(serverSocket, filename.c_str(), filename.size() + 1, 0) == SOCKET_ERROR) {
        std::cerr << "не удалось отправить название файла" << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    
    bool success = false;
    if (command == 'U') {
        success = SendFile(serverSocket, filename);
    }
    else if (command == 'D') {
        success = ReceiveFile(serverSocket, filename);
    }

    
    command = 'Q';
    send(serverSocket, &command, sizeof(command), 0);

    closesocket(serverSocket);
    WSACleanup();

    return success ? 0 : 1;
}