#include <windows.h>
#include <iostream>
#include <string>
#include <locale>
#include <sstream>

using namespace std;

int main() {
   
    setlocale(LC_ALL, "Russian");

    HANDLE hReadParentToChild, hWriteParentToChild; 
    HANDLE hReadChildToParent, hWriteChildToParent; 

    SECURITY_ATTRIBUTES sa = { sizeof(SECURITY_ATTRIBUTES), NULL, TRUE };

    if (!CreatePipe(&hReadParentToChild, &hWriteParentToChild, &sa, 0)) {
        wcerr << L"Ошибка создания первого канала." << endl;
        return 1;
    }

    if (!CreatePipe(&hReadChildToParent, &hWriteChildToParent, &sa, 0)) {
        wcerr << L"Ошибка создания второго канала." << endl;
        CloseHandle(hReadParentToChild);
        CloseHandle(hWriteParentToChild);
        return 1;
    }

    PROCESS_INFORMATION pi;
    STARTUPINFO si = { sizeof(STARTUPINFO) };
    si.dwFlags = STARTF_USESTDHANDLES;
    si.hStdInput = hReadParentToChild;
    si.hStdOutput = hWriteChildToParent;
    si.hStdError = GetStdHandle(STD_ERROR_HANDLE);

    wstring childCmd = L"client10.exe"; 

    if (!CreateProcess(NULL, const_cast<wchar_t*>(childCmd.c_str()), NULL, NULL, TRUE,
        CREATE_NO_WINDOW, NULL, NULL, &si, &pi)) {
        wcerr << L"Ошибка создания дочернего процесса." << endl;
        CloseHandle(hReadParentToChild);
        CloseHandle(hWriteParentToChild);
        CloseHandle(hReadChildToParent);
        CloseHandle(hWriteChildToParent);
        return 1;
    }

    CloseHandle(hReadParentToChild); 
    CloseHandle(hWriteChildToParent); 

    wcout << L"Введите строку: ";
    wstring inputStr;
    getline(wcin, inputStr);

    DWORD bytesWritten;
    if (!WriteFile(hWriteParentToChild, inputStr.c_str(), static_cast<DWORD>((inputStr.size() + 1) * sizeof(wchar_t)), &bytesWritten, NULL)) {
        wcerr << L"Ошибка записи в канал." << endl;
        return 1;
    }
    CloseHandle(hWriteParentToChild); 

    wchar_t buffer[1024];
    DWORD bytesRead;
    if (!ReadFile(hReadChildToParent, buffer, sizeof(buffer) - sizeof(wchar_t), &bytesRead, NULL)) {
        wcerr << L"Ошибка чтения из канала." << endl;
        return 1;
    }
    buffer[bytesRead / sizeof(wchar_t)] = L'\0'; 

    wcout << L"Результат от дочернего процесса: " << buffer << endl;

    CloseHandle(hReadChildToParent);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    return 0;
}