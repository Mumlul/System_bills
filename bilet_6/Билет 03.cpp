#include <iostream>
#include <windows.h> // Для использования Windows API

using namespace std;

int main() {
	setlocale(LC_ALL, "Russian");
	SYSTEMTIME systemTime; // Структура для хранения системного времени

	// Получение текущего системного времени
	GetLocalTime(&systemTime);

	// Вывод даты и времени в удобочитаемом формате
	cout << "Текущая дата и время системы: " << endl;
	cout << "Дата: " << systemTime.wYear << "-"
		<< (systemTime.wMonth < 10 ? "0" : "") << systemTime.wMonth << "-"
		<< (systemTime.wDay < 10 ? "0" : "") << systemTime.wDay << endl;

	cout << "Время: " << (systemTime.wHour < 10 ? "0" : "") << systemTime.wHour << ":"
		<< (systemTime.wMinute < 10 ? "0" : "") << systemTime.wMinute << ":"
		<< (systemTime.wSecond < 10 ? "0" : "") << systemTime.wSecond << "."
		<< systemTime.wMilliseconds << endl;

	return 0;
}