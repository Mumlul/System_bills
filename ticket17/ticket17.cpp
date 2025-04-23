#include <windows.h>
#include <iostream>

HANDLE hMutex;
HANDLE hCondition;
int currentNumber = 0;

DWORD WINAPI Thread1To10(LPVOID lpParam) {
    for (int i = 1; i <= 10; i++) {
        WaitForSingleObject(hMutex, INFINITE); 

        while (currentNumber != i - 1) {
            ReleaseMutex(hMutex);
            Sleep(10);
            WaitForSingleObject(hMutex, INFINITE);
        }

        std::cout << i << " ";
        currentNumber = i;

        ReleaseMutex(hMutex); 
        Sleep(1000); 
    }
    return 0;
}

DWORD WINAPI Thread11To20(LPVOID lpParam) {
    for (int i = 11; i <= 20; i++) {
        WaitForSingleObject(hMutex, INFINITE); 

        while (currentNumber != i - 1) {
            ReleaseMutex(hMutex);
            Sleep(10);
            WaitForSingleObject(hMutex, INFINITE);
        }

        std::cout << i << " ";
        currentNumber = i;

        ReleaseMutex(hMutex); 
        Sleep(1000); 
    }
    return 0;
}

int main() {
    setlocale(LC_ALL, "ru");

    hMutex = CreateMutex(NULL, FALSE, NULL);
    if (hMutex == NULL) {
        std::cerr << "мьютексу свободу! он не раб! имеет право не создаться!" << GetLastError() << std::endl;
        return 1;
    }

    HANDLE threads[2];

    threads[0] = CreateThread(NULL, 0, Thread1To10, NULL, 0, NULL);
    if (threads[0] == NULL) {
        std::cerr << "Ошибка создания потока для 1-10" << GetLastError() << std::endl;
        CloseHandle(hMutex);
        return 1;
    }

    threads[1] = CreateThread(NULL, 0, Thread11To20, NULL, 0, NULL);
    if (threads[1] == NULL) {
        std::cerr << "Ошибка создания потока для 11-20" << GetLastError() << std::endl;
        CloseHandle(threads[0]);
        CloseHandle(hMutex);
        return 1;
    }

    WaitForMultipleObjects(2, threads, TRUE, INFINITE);

    CloseHandle(threads[0]);
    CloseHandle(threads[1]);
    CloseHandle(hMutex);

    return 0;
}