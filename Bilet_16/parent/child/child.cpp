// client.cpp
#include <iostream>
#include <vector>
#include <windows.h>

const wchar_t* PIPE_NAME = L"\\\\.\\pipe\\MaxNumberPipe";

int main() {
    system("chcp 1251 > nul");
    setlocale(LC_ALL, "Russian");

    std::vector<int> numbers(4);
    std::cout << "Введите 4 целых числа:\n";
    for (int i = 0; i < 4; ++i) {
        std::cout << "Число " << i + 1 << ": ";
        std::cin >> numbers[i];
    }

    HANDLE hPipe = CreateFile(
        PIPE_NAME,
        GENERIC_WRITE,
        0,
        NULL,
        OPEN_EXISTING,
        0,
        NULL);

    if (hPipe == INVALID_HANDLE_VALUE) {
        std::cerr << "Не удалось подключиться к серверу. Код ошибки: " << GetLastError() << "\n";
        return 1;
    }

    DWORD bytesWritten;
    if (!WriteFile(hPipe, numbers.data(), numbers.size() * sizeof(int), &bytesWritten, NULL)) {
        std::cerr << "Ошибка отправки данных. Код: " << GetLastError() << "\n";
        CloseHandle(hPipe);
        return 1;
    }

    std::cout << "Числа успешно отправлены на сервер\n";
    CloseHandle(hPipe);
    return 0;
}