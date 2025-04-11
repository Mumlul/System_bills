#include <iostream>

#define PI 3.14159 // Константа PI через #define

int main() {
    setlocale(LC_ALL, "ru");
    double radius;

    while (true) {
        std::cout << "Введите радиус круга: ";

        // Если ввод успешен И радиус > 0 — выходим из цикла
        if (std::cin >> radius && radius > 0) {
            break;
        }
        // Если ввод был некорректным (буквы, отрицательное число или ноль)
        std::cin.clear(); // Сбрасываем флаг ошибки
        std::cin.ignore(1000, '\n'); // Очищаем буфер ввода
        std::cout << "Ошибка! Введите положительное число (>0)\n";
    }

    double area = PI * radius * radius;
    std::cout << "Площадь круга: " << area << std::endl;

    return 0;
}