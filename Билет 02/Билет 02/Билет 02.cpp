#include <iostream>
#include <windows.h>
#include <string>
#include <sstream>

using namespace std;
string ReadRegistryString(HKEY hKey, const string& subKey, const string& valueName) {
    HKEY key;
    if (RegOpenKeyExA(hKey, subKey.c_str(), 0, KEY_READ, &key) != ERROR_SUCCESS) {
        return "";
    }

    char buffer[1024];
    DWORD bufferSize = sizeof(buffer);
    if (RegQueryValueExA(key, valueName.c_str(), nullptr, nullptr, (LPBYTE)buffer, &bufferSize) != ERROR_SUCCESS) {
        RegCloseKey(key);
        return "";
    }

    RegCloseKey(key);
    return string(buffer);
}

DWORD ReadRegistryDword(HKEY hKey, const string& subKey, const string& valueName) {
    HKEY key;
    if (RegOpenKeyExA(hKey, subKey.c_str(), 0, KEY_READ, &key) != ERROR_SUCCESS) {
        return 0;
    }

    DWORD value = 0;
    DWORD size = sizeof(value);
    RegQueryValueExA(key, valueName.c_str(), nullptr, nullptr, (LPBYTE)&value, &size);

    RegCloseKey(key);
    return value;
}

string ConvertUnixTimeToReadable(DWORD unixTime) {
    time_t rawTime = static_cast<time_t>(unixTime);
    struct tm timeInfo;
    localtime_s(&timeInfo, &rawTime);

    char buffer[80];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &timeInfo);
    return string(buffer);
}

int main() {
    SetConsoleOutputCP(CP_UTF8); 
    setlocale(LC_ALL, "Russian");

    cout << "=== Информация о системе ===" << endl;

    string osVersion = ReadRegistryString(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", "ProductName");
    cout << "Операционная система: " << (osVersion.empty() ? "Неизвестно" : osVersion) << endl;

    string installDateStr = ReadRegistryString(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", "InstallDate");
    if (!installDateStr.empty()) {
        stringstream ss(installDateStr);
        unsigned long timestamp;
        if (ss >> timestamp) {
            string readableDate = ConvertUnixTimeToReadable(timestamp);
            cout << "Дата установки: " << readableDate << endl;
        }
        else {
            cout << "Дата установки: Не найдена" << endl;
        }
    }
    else {
        cout << "Дата установки: Не найдена" << endl;
    }

    string processorName = ReadRegistryString(HKEY_LOCAL_MACHINE, "HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0", "ProcessorNameString");
    cout << "Процессор: " << (processorName.empty() ? "Неизвестно" : processorName) << endl;

    string motherboardManufacturer = ReadRegistryString(HKEY_LOCAL_MACHINE, "HARDWARE\\DESCRIPTION\\System\\BIOS", "BaseBoardManufacturer");
    string motherboardProduct = ReadRegistryString(HKEY_LOCAL_MACHINE, "HARDWARE\\DESCRIPTION\\System\\BIOS", "BaseBoardProduct");
    cout << "Материнская плата: "
        << (motherboardManufacturer.empty() ? "Неизвестно" : motherboardManufacturer)
        << " "
        << (motherboardProduct.empty() ? "" : motherboardProduct)
        << endl;

    return 0;
}