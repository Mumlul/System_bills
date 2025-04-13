#include <iostream>
#include <fstream>
#include <windows.h> // Для CreateThread и CRITICAL_SECTION
#include <cstdlib>   // Для rand() и srand()
#include <ctime>     // Для time()

using namespace std;

// Глобальные переменные
const char* FILE_NAME = "data.txt"; // Имя файла для записи/чтения данных
CRITICAL_SECTION fileMutex;         // Мьютекс для синхронизации доступа к файлу

// Функция для первого потока (генерация чисел и запись в файл)
DWORD WINAPI GenerateNumbers(LPVOID lpParam) {
	srand(static_cast<unsigned>(time(0))); // Инициализация генератора случайных чисел

	for (int i = 0; i < 10; ++i) {
		EnterCriticalSection(&fileMutex); // Вход в критическую секцию

		// Открытие файла для записи
		ofstream outFile(FILE_NAME);
		if (!outFile) {
			cerr << "Ошибка открытия файла для записи!" << endl;
			LeaveCriticalSection(&fileMutex); // Выход из критической секции
			return 1;
		}

		cout << "Запись чисел в файл: ";
		for (int j = 0; j < 10; ++j) {
			int number = rand() % 100; // Генерация случайного числа от 0 до 99
			outFile << number << " ";
			cout << number << " ";
		}
		cout << endl;

		outFile.close(); // Закрытие файла
		LeaveCriticalSection(&fileMutex); // Выход из критической секции

		Sleep(3000); // Ожидание 3 секунды
	}

	return 0;
}

// Функция для второго потока (чтение чисел из файла и вычисление суммы)
DWORD WINAPI CalculateSum(LPVOID lpParam) {
	while (true) {
		EnterCriticalSection(&fileMutex); // Вход в критическую секцию

		// Открытие файла для чтения
		ifstream inFile(FILE_NAME);
		if (!inFile) {
			LeaveCriticalSection(&fileMutex); // Выход из критической секции
			continue; // Если файл не существует, продолжаем цикл
		}

		int sum = 0;
		int number;

		cout << "Чтение чисел из файла: ";
		while (inFile >> number) {
			sum += number;
			cout << number << " ";
		}
		cout << endl;

		inFile.close(); // Закрытие файла
		LeaveCriticalSection(&fileMutex); // Выход из критической секции

		cout << "Сумма чисел: " << sum << endl;

		Sleep(3000); // Ожидание 3 секунды
	}

	return 0;
}

int main() {
	setlocale(LC_ALL, "Russian");
	// Инициализация мьютекса
	InitializeCriticalSection(&fileMutex);

	// Создание потоков
	HANDLE hThread1 = CreateThread(NULL, 0, GenerateNumbers, NULL, 0, NULL);
	HANDLE hThread2 = CreateThread(NULL, 0, CalculateSum, NULL, 0, NULL);

	if (hThread1 == NULL || hThread2 == NULL) {
		cerr << "Ошибка создания потоков!" << endl;
		return 1;
	}

	// Ожидание завершения первого потока
	WaitForSingleObject(hThread1, INFINITE);

	// Завершение второго потока (принудительно, так как он работает бесконечно)
	TerminateThread(hThread2, 0);

	// Закрытие дескрипторов потоков
	CloseHandle(hThread1);
	CloseHandle(hThread2);

	// Удаление мьютекса
	DeleteCriticalSection(&fileMutex);

	cout << "Программа завершена." << endl;

	return 0;
}