#include <windows.h>
#include <iostream>
#include <vector>

int main() {
    setlocale(LC_ALL, "ru");
    // Получаем дескрипторы каналов
    HANDLE hStdIn = GetStdHandle(STD_INPUT_HANDLE);
    HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);

    if (hStdIn == INVALID_HANDLE_VALUE || hStdOut == INVALID_HANDLE_VALUE) {
        std::cerr << "Ошибка GetStdHandle" << std::endl;
        return 1;
    }

    // Читаем размер массива
    int arraySize;
    DWORD bytesRead;
    if (!ReadFile(hStdIn, &arraySize, sizeof(arraySize), &bytesRead, NULL)) {
        std::cerr << "Ошибка чтения размера массива" << std::endl;
        return 1;
    }

    // Читаем сам массив
    std::vector<int> receivedArray(arraySize);
    if (!ReadFile(hStdIn, receivedArray.data(), arraySize * sizeof(int), &bytesRead, NULL)) {
        std::cerr << "Ошибка чтения данных массива" << std::endl;
        return 1;
    }

    // Вычисляем квадраты чисел
    std::vector<int> squaredArray(arraySize);
    for (int i = 0; i < arraySize; ++i) {
        squaredArray[i] = receivedArray[i] * receivedArray[i];
    }

    // Отправляем результаты обратно родительскому процессу
    DWORD bytesWritten;
    if (!WriteFile(hStdOut, squaredArray.data(), arraySize * sizeof(int), &bytesWritten, NULL)) {
        std::cerr << "Ошибка записи квадратов массива" << std::endl;
        return 1;
    }

    return 0;
}