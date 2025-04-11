#include <windows.h>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <chrono>

using namespace std;

struct ThreadData {
    int* array;
    int size;
    __int64 result;
    double time_ms;
};

DWORD WINAPI SumThread(LPVOID lpParam) {
    auto start_time = chrono::high_resolution_clock::now();

    ThreadData* data = (ThreadData*)lpParam;
    __int64 sum = 0;

    for (int i = 0; i < data->size; ++i) {
        sum += data->array[i];
    }

    data->result = sum;

    auto end_time = chrono::high_resolution_clock::now();
    chrono::duration<double, milli> elapsed = end_time - start_time;
    data->time_ms = elapsed.count();

    return 0;
}

DWORD WINAPI ProductThread(LPVOID lpParam) {
    auto start_time = chrono::high_resolution_clock::now();

    ThreadData* data = (ThreadData*)lpParam;
    __int64 product = 1;

    for (int i = 0; i < data->size; ++i) {
        product *= data->array[i];
    }

    data->result = product;

    auto end_time = chrono::high_resolution_clock::now();
    chrono::duration<double, milli> elapsed = end_time - start_time;
    data->time_ms = elapsed.count();

    return 0;
}

int main() {
    setlocale(LC_ALL, "Russian");
    srand(time(nullptr));

    int size;
    cout << "Введите размер массива: ";
    cin >> size;

    if (size <= 0) {
        cerr << "Ошибка: размер массива должен быть положительным числом." << endl;
        return 1;
    }

    int* array = new int[size];
    for (int i = 0; i < size; ++i) {
        array[i] = rand() % 201 - 100; 
        cout << array[i] << " "; 
    }

    ThreadData sumData{ array, size, 0, 0.0 };
    ThreadData productData{ array, size, 0, 0.0 };

    HANDLE hSumThread = CreateThread(nullptr, 0, SumThread, &sumData, 0, nullptr);

    HANDLE hProductThread = CreateThread(nullptr, 0, ProductThread, &productData, 0, nullptr);

    if (hSumThread == nullptr || hProductThread == nullptr) {
        cerr << "Ошибка при создании потоков." << endl;
        delete[] array;
        return 1;
    }

    WaitForSingleObject(hSumThread, INFINITE);
    WaitForSingleObject(hProductThread, INFINITE);

    CloseHandle(hSumThread);
    CloseHandle(hProductThread);

    cout << "\nРезультаты:" << endl;
    cout << "Сумма элементов: " << sumData.result
        << " (время вычисления: " << sumData.time_ms << " мс)" << endl;
    cout << "Произведение элементов: " << productData.result
        << " (время вычисления: " << productData.time_ms << " мс)" << endl;

    delete[] array;

    return 0;
}