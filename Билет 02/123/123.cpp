#include <windows.h>
#include <iostream>
#include <string>
#include <ctime>

std::string ReadRegistryString(HKEY rootKey, const std::string& subKey, const std::string& valueName) {
    HKEY hKey;
    DWORD size = 0;
    std::string result;

    if (RegOpenKeyExA(rootKey, subKey.c_str(), 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        // Получаем размер данных
        if (RegQueryValueExA(hKey, valueName.c_str(), nullptr, nullptr, nullptr, &size) == ERROR_SUCCESS) {
            // Читаем строку из реестра
            char* buffer = new char[size];
            if (RegQueryValueExA(hKey, valueName.c_str(), nullptr, nullptr, (LPBYTE)buffer, &size) == ERROR_SUCCESS) {
                result = buffer;
            }
            delete[] buffer;
        }
        RegCloseKey(hKey);
    }

    // Удаляем нулевые символы в конце строки
    while (!result.empty() && result.back() == '\0') {
        result.pop_back();
    }

    return result;
}

DWORD ReadRegistryDword(HKEY rootKey, const std::string& subKey, const std::string& valueName) {
    HKEY hKey;
    DWORD value = 0;
    DWORD size = sizeof(DWORD);

    if (RegOpenKeyExA(rootKey, subKey.c_str(), 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        RegQueryValueExA(hKey, valueName.c_str(), nullptr, nullptr, (LPBYTE)&value, &size);
        RegCloseKey(hKey);
    }

    return value;
}

std::string FormatInstallDate(DWORD timestamp) {
    if (timestamp == 0) return "Неизвестно";

    time_t rawTime = static_cast<time_t>(timestamp);
    struct tm timeInfo;
    localtime_s(&timeInfo, &rawTime);

    char buffer[80];
    strftime(buffer, sizeof(buffer), "%d.%m.%Y %H:%M:%S", &timeInfo);

    return buffer;
}

int main() {
    // Устанавливаем русскую кодировку консоли
    SetConsoleOutputCP(1251);
    SetConsoleCP(1251);
    setlocale(LC_ALL, "Russian");

    const std::string regPath = "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion";

    // Получаем информацию из реестра
    std::string productName = ReadRegistryString(HKEY_LOCAL_MACHINE, regPath, "ProductName");
    std::string displayVersion = ReadRegistryString(HKEY_LOCAL_MACHINE, regPath, "DisplayVersion");
    std::string buildNumber = ReadRegistryString(HKEY_LOCAL_MACHINE, regPath, "CurrentBuild");
    DWORD installDate = ReadRegistryDword(HKEY_LOCAL_MACHINE, regPath, "InstallDate");

    // Формируем полную версию ОС
    std::string fullVersion = productName;
    if (!displayVersion.empty()) {
        fullVersion += " " + displayVersion;
    }
    if (!buildNumber.empty()) {
        fullVersion += " (Сборка " + buildNumber + ")";
    }

    // Выводим информацию
    std::cout << "Информация об операционной системе:" << std::endl;
    std::cout << "----------------------------------" << std::endl;
    std::cout << "Название: " << (productName.empty() ? "Неизвестно" : fullVersion) << std::endl;
    std::cout << "Дата установки: " << FormatInstallDate(installDate) << std::endl;

    system("pause");
    return 0;
}