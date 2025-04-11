// client.cpp
#include <iostream>
#include <string>
#include <windows.h>
#include <algorithm>
#include <cctype>

const wchar_t* PIPE_NAME = L"\\\\.\\pipe\\MyNamedPipe";

size_t count_non_space_chars(const std::string& str) {
    return std::count_if(str.begin(), str.end(),
        [](char c) { return !std::isspace(static_cast<unsigned char>(c)); });
}

int main() {
    system("chcp 1251 > nul");
    setlocale(LC_ALL, "Russian");

    std::string input;
    std::cout << "Введите строку для отправки: ";
    std::getline(std::cin, input);

    HANDLE hPipe = CreateFile(
        PIPE_NAME,
        GENERIC_WRITE,
        0,
        NULL,
        OPEN_EXISTING,
        0,
        NULL);

    if (hPipe == INVALID_HANDLE_VALUE) {
        std::cerr << "Не удалось подключиться к каналу. Убедитесь, что сервер запущен. Код ошибки: " << GetLastError() << "\n";
        return 1;
    }

    DWORD bytesWritten;
    size_t length = input.size();
    if (!WriteFile(hPipe, &length, sizeof(length), &bytesWritten, NULL)) {
        std::cerr << "Ошибка отправки длины. Код: " << GetLastError() << "\n";
        CloseHandle(hPipe);
        return 1;
    }

    if (!WriteFile(hPipe, input.c_str(), static_cast<DWORD>(length), &bytesWritten, NULL)) {
        std::cerr << "Ошибка отправки строки. Код: " << GetLastError() << "\n";
        CloseHandle(hPipe);
        return 1;
    }

    // Выводим информацию о количестве символов без пробелов
    size_t char_count = count_non_space_chars(input);
    std::cout << "Данные успешно отправлены\n";
    std::cout << "Количество символов: " << char_count << "\n";

    CloseHandle(hPipe);
    return 0;
}