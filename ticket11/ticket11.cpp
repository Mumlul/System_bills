#include <windows.h>
#include <iostream>

HANDLE hMutex;
bool helloPrinted = false;

DWORD WINAPI ThreadHello(LPVOID lpParam) {
    WaitForSingleObject(hMutex, INFINITE); 

    std::cout << "Hello";
    helloPrinted = true;

    ReleaseMutex(hMutex); 
    return 0;
}

DWORD WINAPI ThreadWorld(LPVOID lpParam) {

    while (true) {
        WaitForSingleObject(hMutex, INFINITE); 
        if (helloPrinted) {
            std::cout << " World" << std::endl;
            ReleaseMutex(hMutex); 
            break;
        }
        ReleaseMutex(hMutex); 
        Sleep(10);
    }
    return 0;
}

int main() {
    setlocale(LC_ALL, "ru");

    hMutex = CreateMutex(NULL, FALSE, NULL);
    if (hMutex == NULL) {
        std::cerr << "Что-то случилось с созданием мьютекса" << GetLastError() << std::endl;
        return 1;
    }

    HANDLE threads[2];

    threads[0] = CreateThread(NULL, 0, ThreadHello, NULL, 0, NULL);
    if (threads[0] == NULL) {
        std::cerr << "Ошибка создания потока для хелло: " << GetLastError() << std::endl;
        CloseHandle(hMutex);
        return 1;
    }

    threads[1] = CreateThread(NULL, 0, ThreadWorld, NULL, 0, NULL);
    if (threads[1] == NULL) {
        std::cerr << "Ошибка создания потока для ворлд" << GetLastError() << std::endl;
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
