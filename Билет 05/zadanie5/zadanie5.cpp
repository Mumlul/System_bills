#include <windows.h>
#include <iostream>
#include <string>
#include <tchar.h>

using namespace std;

int main()
{
    setlocale(LC_ALL, "ru");
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    int processCount;
    cout << "Введите количество процессов для создания: ";
    cin >> processCount;

    if (processCount <= 0)
    {
        cout << "Количество процессов должно быть положительным числом!" << endl;
        return 1;
    }

    // Получаем путь к текущему исполняемому файлу
    TCHAR szPath[MAX_PATH];
    GetModuleFileName(NULL, szPath, MAX_PATH);

    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    for (int i = 0; i < processCount; ++i)
    {
        
        if (!CreateProcess(
            szPath,                 
            NULL,                   
            NULL,                   
            NULL,                   
            FALSE,                  
            CREATE_NEW_CONSOLE,     
            NULL,                   
            NULL,                
            &si,                    
            &pi)                    
            )
        {
            cout << "Ошибка при создании процесса (" << GetLastError() << ")" << endl;
            continue;
        }

        
        cout << "Создан процесс с ID: " << pi.dwProcessId << endl;

       
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }

    cout << "Всего создано процессов: " << processCount << endl;
    system("pause");
    return 0;
}