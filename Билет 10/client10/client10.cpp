#include <windows.h>
#include <iostream>
#include <string>
#include <algorithm>
#include <cctype>
#include <locale>
#include <sstream>

using namespace std;

int main() {
    
    setlocale(LC_ALL, "Russian");

    wchar_t buffer[1024];
    DWORD bytesRead;
    if (!ReadFile(GetStdHandle(STD_INPUT_HANDLE), buffer, sizeof(buffer) - sizeof(wchar_t), &bytesRead, NULL)) {
        wcerr << L"Ошибка чтения из канала." << endl;
        return 1;
    }
    buffer[bytesRead / sizeof(wchar_t)] = L'\0'; 

    wstring str(buffer);
    transform(str.begin(), str.end(), str.begin(), towlower);
    int count = str.length();

    wostringstream resultStream;
    resultStream << L"Строка: " << str << L", Количество символов: " << count;

    wstring result = resultStream.str();
    DWORD bytesWritten;
    if (!WriteFile(GetStdHandle(STD_OUTPUT_HANDLE), result.c_str(), static_cast<DWORD>((result.size() + 1) * sizeof(wchar_t)), &bytesWritten, NULL)) {
        wcerr << L"Ошибка записи в канал." << endl;
        return 1;
    }

    return 0;
}