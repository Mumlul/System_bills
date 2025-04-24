#include <iostream>
#include <vector>
#include <fstream>
#include <random>
#include <string>
#include <direct.h>  // для _mkdir
#include <cstdlib>   // для system()

int main() {
    setlocale(LC_ALL, "Rus");
    int size;
    std::cout << "Введите размер массива: ";
    std::cin >> size;

    if (size <= 0) {
        std::cerr << "Размер массива должен быть положительным числом.\n";
        return 1;
    }

    // Генерация случайных чисел
    std::vector<double> numbers(size);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dist(0.0, 100.0);

    for (int i = 0; i < size; ++i) {
        numbers[i] = dist(gen);
    }

    // Создание каталога с помощью _mkdir (Windows)
    std::string dirName = "output_data";
    if (_mkdir(dirName.c_str()) != 0) {
        std::cerr << "Каталог возможно уже существует или не удалось создать.\n";
    }

    // Запись в файл
    std::string filePath = dirName + "\\data.txt";
    std::ofstream outFile(filePath);
    if (!outFile) {
        std::cerr << "Не удалось открыть файл для записи.\n";
        return 1;
    }

    for (double num : numbers) {
        outFile << num << '\n';
    }

    outFile.close();
    std::cout << "Массив успешно сохранен в " << filePath << "\n";

    return 0;
}
