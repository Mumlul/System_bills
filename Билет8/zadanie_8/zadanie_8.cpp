#include <windows.h>
#include <iostream>
#include <fstream>
#include <ctime>

using namespace std;

// Глобальные переменные
HANDLE hMutex;
const char* filename = "numbers.txt";
const int THREADS_COUNT = 3;
const int NUMBERS_PER_THREAD = 5;

// Функция потока
DWORD WINAPI ThreadFunction(LPVOID lpParam) {
    int threadId = *(int*)lpParam;

    for (int i = 0; i < NUMBERS_PER_THREAD; ++i) {
        // Генерация случайного числа от 0 до 100
        int number = rand() % 101;

        // Ожидание мьютекса
        WaitForSingleObject(hMutex, INFINITE);

        // Открытие файла для добавления
        ofstream outfile(filename, ios_base::app);
        if (outfile.is_open()) {
            outfile << "Поток " << threadId << ": " << number << endl;
            outfile.close();
            cout << "Поток " << threadId << " записал число: " << number << endl;
        }
        else {
            cerr << "Ошибка открытия файла в потоке " << threadId << endl;
        }

        // Освобождение мьютекса
        ReleaseMutex(hMutex);

        // Небольшая задержка для наглядности
        Sleep(100);
    }

    return 0;
}

int main() {
    setlocale(LC_ALL, "ru");
    // Инициализация генератора случайных чисел
    srand(static_cast<unsigned int>(time(NULL)));

    // Создание мьютекса
    hMutex = CreateMutex(NULL, FALSE, NULL);
    if (hMutex == NULL) {
        cerr << "Ошибка создания мьютекса" << endl;
        return 1;
    }

    // Очистка файла перед началом работы
    ofstream outfile(filename, ios_base::trunc);
    outfile.close();

    // Создание потоков
    HANDLE threads[THREADS_COUNT];
    int threadIds[THREADS_COUNT];

    for (int i = 0; i < THREADS_COUNT; ++i) {
        threadIds[i] = i + 1;
        threads[i] = CreateThread(
            NULL,                   // Атрибуты безопасности по умолчанию
            0,                      // Размер стека по умолчанию
            ThreadFunction,         // Функция потока
            &threadIds[i],          // Аргумент для функции потока
            0,                      // Флаги создания по умолчанию
            NULL                    // Не возвращаем идентификатор потока
        );

        if (threads[i] == NULL) {
            cerr << "Ошибка создания потока " << i + 1 << endl;
            CloseHandle(hMutex);
            return 1;
        }
    }

    // Ожидание завершения всех потоков
    WaitForMultipleObjects(THREADS_COUNT, threads, TRUE, INFINITE);

    // Закрытие дескрипторов
    for (int i = 0; i < THREADS_COUNT; ++i) {
        CloseHandle(threads[i]);
    }

    CloseHandle(hMutex);

    cout << "Все потоки завершили работу. Результаты записаны в " << filename << endl;

    return 0;
}