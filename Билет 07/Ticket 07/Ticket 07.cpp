//Разработать консольное приложение на C++, которое запрашивает у пользователя строку и подсчитывает количество гласных и согласных букв,
//результат работы нужно вывести на экран, а также сохранить в файл исходные данные и результат работы программы.

#include <iostream>
#include <fstream>
#include <string>
#include <cctype> // для функции tolower()
#include <vector>

using namespace std;

// Функция для проверки, является ли символ гласной буквой
bool isVowel(char ch) {
    ch = tolower(ch); // приводим к нижнему регистру для упрощения проверки
    vector<char> vowels = { 'a', 'e', 'i', 'o', 'u', 'y' };

    for (char vowel : vowels) {
        if (ch == vowel) {
            return true;
        }
    }

    return false;
}

// Функция для проверки, является ли символ согласной буквой
bool isConsonant(char ch) {
    ch = tolower(ch);

    // Если это не буква, то это не согласная
    if (!isalpha(ch)) {
        return false;
    }

    // Если это не гласная, то это согласная
    return !isVowel(ch);
}

// Функция для сохранения результатов в файл
void saveResultsToFile(const string& input, int vowelCount, int consonantCount) {
    ofstream outFile("results.txt");

    if (outFile.is_open()) {
        outFile << "Исходная строка: " << input << endl;
        outFile << "Количество гласных букв: " << vowelCount << endl;
        outFile << "Количество согласных букв: " << consonantCount << endl;
        outFile.close();
        cout << "Результаты сохранены в файл 'results.txt'" << endl;
    }
    else {
        cerr << "Не удалось открыть файл для записи!" << endl;
    }
}

int main() {
    // Устанавливаем локаль для поддержки русских символов
    setlocale(LC_ALL, "Russian");

    string input;
    int vowelCount = 0;
    int consonantCount = 0;

    cout << "Введите строку для анализа на Egl языке: ";
    getline(cin, input);

    // Подсчёт гласных и согласных
    for (char ch : input) {
        if (isVowel(ch)) {
            vowelCount++;
        }
        else if (isConsonant(ch)) {
            consonantCount++;
        }
    }

    // Вывод результатов на экран
    cout << "\nРезультаты анализа строки:" << endl;
    cout << "Исходная строка: " << input << endl;
    cout << "Количество гласных букв: " << vowelCount << endl;
    cout << "Количество согласных букв: " << consonantCount << endl;

    // Сохранение результатов в файл
    saveResultsToFile(input, vowelCount, consonantCount);

    return 0;
}

