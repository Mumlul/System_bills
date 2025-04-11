#include <iostream>
#include <string>
#include <limits>

using namespace std;

//Это - вариант, где НЕ учитывается ввод дробного числа
void checkEvenOdd(int number) {
    if (number == 0) {
        throw runtime_error("Ошибка: введен ноль. Ноль не является ни четным, ни нечетным числом.");
    }

    if (number % 2 == 0) {
        cout << "Число " << number << " является четным." << endl;
    }
    else {
        cout << "Число " << number << " является нечетным." << endl;
    }
}

int main() {
    setlocale(LC_ALL, "Russian");

    cout << "Программа для определения четности числа." << endl;

    while (true) {
        int number;
        cout << "\nВведите целое число (или 'q' для выхода): ";

        if (!(cin >> number)) {
            cin.clear();

            string input;
            cin >> input;

            if (input == "q" || input == "Q") {
                cout << "Выход из программы." << endl;
                break;
            }

            cout << "Ошибка: введены недопустимые символы. Пожалуйста, введите целое число." << endl;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }

        try {
            checkEvenOdd(number);
        }
        catch (const runtime_error& e) {
            cout << e.what() << endl;
        }
    }

    return 0;
}

////////////////Это - вариант, где УЧИТЫВАЕТСЯ ввод дробного числа
// 
//void checkEvenOdd(int number) {
//    if (number == 0) {
//        throw runtime_error("Ошибка: введен ноль. Ноль не является ни четным, ни нечетным числом.");
//    }
//
//    if (number % 2 == 0) {
//        cout << "Число " << number << " является четным." << endl;
//    }
//    else {
//        cout << "Число " << number << " является нечетным." << endl;
//    }
//}
//
//bool isInteger(const string& input) {
//    istringstream iss(input);
//    double num;
//    iss >> num;
//
//    // Проверяем, что извлекли число и что после него нет символов (кроме пробелов)
//    return (iss.eof() && num == static_cast<int>(num));
//}
//
//int main() {
//    setlocale(LC_ALL, "Russian");
//
//    cout << "Программа для определения четности числа." << endl;
//
//    while (true) {
//        string input;
//        cout << "\nВведите целое число (или 'q' для выхода): ";
//        cin >> input;
//
//        if (input == "q" || input == "Q") {
//            cout << "Выход из программы." << endl;
//            break;
//        }
//
//        if (!isInteger(input)) {
//            cout << "Ошибка: введено не целое число. Пожалуйста, введите целое число." << endl;
//            cin.ignore(numeric_limits<streamsize>::max(), '\n');
//            continue;
//        }
//
//        istringstream iss(input);
//        int number;
//        iss >> number;
//
//        try {
//            checkEvenOdd(number);
//        }
//        catch (const runtime_error& e) {
//            cout << e.what() << endl;
//        }
//    }
//
//    return 0;
//}
