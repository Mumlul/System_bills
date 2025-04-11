// server.cpp
#include <iostream>
#include <fstream>
#include <vector>
#include <windows.h>
#include <algorithm>

const wchar_t* PIPE_NAME = L"\\\\.\\pipe\\MaxNumberPipe";
const char* OUTPUT_FILE = "max_number.txt";

int main() {
    system("chcp 1251 > nul");
    setlocale(LC_ALL, "Russian");

    std::cout << "Сервер запущен. Ожидание чисел...\n";

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
        std::cout << "Клиент подключен. Получение данных...\n";

        std::vector<int> numbers(4);
        DWORD bytesRead;

        if (!ReadFile(hPipe, numbers.data(), numbers.size() * sizeof(int), &bytesRead, NULL)) {
            std::cerr << "Ошибка чтения данных. Код: " << GetLastError() << "\n";
            CloseHandle(hPipe);
            return 1;
        }

        // Находим максимальное число
        int max_num = *std::max_element(numbers.begin(), numbers.end());

        // Записываем результат в файл
        std::ofstream out(OUTPUT_FILE);
        if (out.is_open()) {
            out << "Полученные числа: ";
            for (int num : numbers) {
                out << num << " ";
            }
            out << "\nМаксимальное число: " << max_num << "\n";
            out.close();
            std::cout << "Результат записан в файл '" << OUTPUT_FILE << "'\n";
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