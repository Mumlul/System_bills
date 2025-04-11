// server.cpp
#include <iostream>
#include <fstream>
#include <string>
#include <windows.h>
#include <algorithm>
#include <cctype>

const wchar_t* PIPE_NAME = L"\\\\.\\pipe\\MyNamedPipe";
const char* OUTPUT_FILE = "output.txt";

size_t count_non_space_chars(const std::string& str) {
    return std::count_if(str.begin(), str.end(),
        [](char c) { return !std::isspace(static_cast<unsigned char>(c)); });
}

int main() {
    system("chcp 1251 > nul");
    setlocale(LC_ALL, "Russian");

    std::cout << "Сервер запущен. Ожидание подключения клиента...\n";

    HANDLE hPipe = CreateNamedPipe(
        PIPE_NAME,
        PIPE_ACCESS_INBOUND,
        PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
        1,
        1024,
        1024,
        0,
        NULL);

    if (hPipe == INVALID_HANDLE_VALUE) {
        std::cerr << "Ошибка создания канала. Код: " << GetLastError() << "\n";
        return 1;
    }

    BOOL connected = ConnectNamedPipe(hPipe, NULL) ? TRUE : (GetLastError() == ERROR_PIPE_CONNECTED);

    if (connected) {
        std::cout << "Клиент подключен. Ожидание данных...\n";

        DWORD bytesRead;
        size_t length;
        if (!ReadFile(hPipe, &length, sizeof(length), &bytesRead, NULL)) {
            std::cerr << "Ошибка чтения длины. Код: " << GetLastError() << "\n";
            CloseHandle(hPipe);
            return 1;
        }

        char* buffer = new char[length + 1];
        if (!ReadFile(hPipe, buffer, static_cast<DWORD>(length), &bytesRead, NULL)) {
            std::cerr << "Ошибка чтения строки. Код: " << GetLastError() << "\n";
            delete[] buffer;
            CloseHandle(hPipe);
            return 1;
        }
        buffer[length] = '\0';
        std::string received(buffer);
        delete[] buffer;

        // Подсчет символов без пробелов
        size_t char_count = count_non_space_chars(received);

        std::ofstream out(OUTPUT_FILE);
        if (out.is_open()) {
            out << "Полученная строка: " << received << "\n";
            out << "Количество символов: " << char_count << "\n";
            out.close();
            std::cout << "Данные записаны в '" << OUTPUT_FILE << "'\n";
        }
        else {
            std::cerr << "Ошибка записи в файл\n";
        }
    }
    else {
        std::cerr << "Ошибка подключения клиента. Код: " << GetLastError() << "\n";
    }

    CloseHandle(hPipe);
    return 0;
}