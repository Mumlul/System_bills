#include <windows.h>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <string>

// Функция для генерации случайного массива
std::vector<int> generateRandomArray(int size) {
    std::vector<int> arr(size);
    for (int i = 0; i < size; ++i) {
        arr[i] = rand() % 101; // Генерация чисел от 0 до 100
    }
    return arr;
}

// Функция для печати массива
void printArray(const std::vector<int>& arr, const std::string& title) {
    std::cout << title << ": [";
    for (size_t i = 0; i < arr.size(); ++i) {
        std::cout << arr[i];
        if (i != arr.size() - 1) {
            std::cout << ", ";
        }
    }
    std::cout << "]" << std::endl;
}

int main() {
    setlocale(LC_ALL, "ru");
    // Инициализация генератора случайных чисел
    srand(static_cast<unsigned>(time(nullptr)));

    // Запрос размера массива у пользователя
    int arraySize;
    std::cout << "Введите размер массива: ";
    std::cin >> arraySize;

    if (arraySize <= 0) {
        std::cerr << "Размер массива должен быть положительным числом!" << std::endl;
        return 1;
    }

    // Создаем каналы для обмена данными
    HANDLE hChildStd_IN_Rd = NULL;
    HANDLE hChildStd_IN_Wr = NULL;
    HANDLE hChildStd_OUT_Rd = NULL;
    HANDLE hChildStd_OUT_Wr = NULL;

    SECURITY_ATTRIBUTES saAttr;
    saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
    saAttr.bInheritHandle = TRUE;
    saAttr.lpSecurityDescriptor = NULL;

    // Создаем канал для записи в дочерний процесс
    if (!CreatePipe(&hChildStd_IN_Rd, &hChildStd_IN_Wr, &saAttr, 0)) {
        std::cerr << "Ошибка создания канала для ввода" << std::endl;
        return 1;
    }

    // Создаем канал для чтения из дочернего процесса
    if (!CreatePipe(&hChildStd_OUT_Rd, &hChildStd_OUT_Wr, &saAttr, 0)) {
        std::cerr << "Ошибка создания канала для вывода" << std::endl;
        return 1;
    }

    // Настраиваем атрибуты процесса
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(STARTUPINFO));
    si.cb = sizeof(STARTUPINFO);
    si.hStdError = GetStdHandle(STD_ERROR_HANDLE);
    si.hStdOutput = hChildStd_OUT_Wr;
    si.hStdInput = hChildStd_IN_Rd;
    si.dwFlags |= STARTF_USESTDHANDLES;

    ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));

    // Создаем дочерний процесс
    TCHAR cmdLine[] = TEXT("\"ChildProcess.exe\"");
    if (!CreateProcess(NULL, cmdLine, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi)) {
        std::cerr << "Ошибка создания процесса (" << GetLastError() << ")" << std::endl;
        return 1;
    }

    // Закрываем ненужные дескрипторы в родительском процессе
    CloseHandle(hChildStd_IN_Rd);
    CloseHandle(hChildStd_OUT_Wr);

    // Генерируем массив случайных чисел
    std::vector<int> randomArray = generateRandomArray(arraySize);
    printArray(randomArray, "Исходный массив");

    // Отправляем размер массива дочернему процессу
    DWORD bytesWritten;
    if (!WriteFile(hChildStd_IN_Wr, &arraySize, sizeof(arraySize), &bytesWritten, NULL)) {
        std::cerr << "Ошибка записи размера массива" << std::endl;
        return 1;
    }

    // Отправляем сам массив дочернему процессу
    if (!WriteFile(hChildStd_IN_Wr, randomArray.data(), arraySize * sizeof(int), &bytesWritten, NULL)) {
        std::cerr << "Ошибка записи данных массива" << std::endl;
        return 1;
    }

    // Закрываем дескриптор записи в дочерний процесс
    CloseHandle(hChildStd_IN_Wr);

    // Читаем результаты из дочернего процесса
    std::vector<int> squaredArray(arraySize);
    DWORD bytesRead;
    if (!ReadFile(hChildStd_OUT_Rd, squaredArray.data(), arraySize * sizeof(int), &bytesRead, NULL)) {
        std::cerr << "Ошибка чтения квадратов массива" << std::endl;
        return 1;
    }

    // Выводим результаты
    printArray(squaredArray, "Массив квадратов");

    // Закрываем оставшиеся дескрипторы
    CloseHandle(hChildStd_OUT_Rd);

    // Ждем завершения дочернего процесса
    WaitForSingleObject(pi.hProcess, INFINITE);

    // Закрываем дескрипторы процесса и потока
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    return 0;
}