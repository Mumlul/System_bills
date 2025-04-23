#include <windows.h>
#include <iostream>
#include <string>
#include <tlhelp32.h>
#include <vector>

using namespace std;


// Функция для запуска приложения
void LaunchApplication(const wstring& appName) {
    SHELLEXECUTEINFO sei = { sizeof(sei) };
    sei.lpVerb = L"open";
    sei.lpFile = appName.c_str();
    sei.nShow = SW_SHOWNORMAL;

    if (!ShellExecuteEx(&sei)) {
        wcerr << L"Не удалось запустить " << appName << L". Ошибка: " << GetLastError() << endl;
    }
    else {
        wcout << L"Приложение " << appName << L" успешно запущено." << endl;
    }
}

// Функция для закрытия приложения по имени процесса
void CloseApplication(const wstring& processName) {
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        wcerr << L"Ошибка создания снимка процессов." << endl;
        return;
    }

    PROCESSENTRY32 pe;
    pe.dwSize = sizeof(PROCESSENTRY32);

    bool found = false;
    if (Process32First(hSnapshot, &pe)) {
        do {
            if (_wcsicmp(pe.szExeFile, processName.c_str()) == 0) {
                HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, pe.th32ProcessID);
                if (hProcess != NULL) {
                    TerminateProcess(hProcess, 0);
                    CloseHandle(hProcess);
                    wcout << L"Приложение " << processName << L" успешно закрыто." << endl;
                    found = true;
                }
            }
        } while (Process32Next(hSnapshot, &pe));
    }

    CloseHandle(hSnapshot);

    if (!found) {
        wcout << L"Приложение " << processName << L" не найдено среди запущенных процессов." << endl;
    }
}

// Функция для отображения меню
void ShowMenu() {
    wcout << L"\nМеню управления Microsoft Office:\n";
    wcout << L"1. Запустить Microsoft Excel\n";
   wcout << L"2. Запустить Microsoft Word\n";
    wcout << L"3. Запустить Microsoft PowerPoint\n";
    wcout << L"4. Закрыть Microsoft Excel\n";
    wcout << L"5. Закрыть Microsoft Word\n";
    wcout << L"6. Закрыть Microsoft PowerPoint\n";
    wcout << L"7. Выйти из программы\n";
    wcout << L"Выберите действие: ";
}

int main() {
    // Установка локали для поддержки русского языка
    setlocale(LC_ALL, "ru");
    SetConsoleOutputCP(1251);
    SetConsoleCP(1251);

    int choice;
    do {
        ShowMenu();
        wcin >> choice;

        switch (choice) {
        case 1:
            LaunchApplication(L"EXCEL.EXE");
            break;
        case 2:
            LaunchApplication(L"WINWORD.EXE");
            break;
        case 3:
            LaunchApplication(L"POWERPNT.EXE");
            break;
        case 4:
            CloseApplication(L"EXCEL.EXE");
            break;
        case 5:
            CloseApplication(L"WINWORD.EXE");
            break;
        case 6:
            CloseApplication(L"POWERPNT.EXE");
            break;
        case 7:
            wcout << L"Выход из программы.\n";
            break;
        default:
            wcout << L"Неверный выбор. Попробуйте снова.\n";
        }
    } while (choice != 7);

    return 0;
}