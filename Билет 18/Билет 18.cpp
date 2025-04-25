#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <openssl/aes.h>
#include <openssl/evp.h>
#include <openssl/sha.h>
#include <iomanip>
#include <sstream>

using namespace std;

// Функция для хэширования пароля с помощью SHA-256
vector<unsigned char> hashPassword(const string& password) {
    vector<unsigned char> hash(SHA256_DIGEST_LENGTH);
    SHA256((const unsigned char*)password.c_str(), password.length(), hash.data());
    return hash;
}

// Функция для шифрования файла
string encryptFile(const string& inputFile, const vector<unsigned char>& key) {
    // Создаем имя для зашифрованного файла
    string outputFile = inputFile + ".enc";

    ifstream in(inputFile, ios::binary);
    ofstream out(outputFile, ios::binary);

    if (!in) {
        cerr << "Ошибка открытия входного файла!" << endl;
        return "";
    }
    if (!out) {
        cerr << "Ошибка создания зашифрованного файла!" << endl;
        return "";
    }

    // Инициализация AES
    AES_KEY aesKey;
    AES_set_encrypt_key(key.data(), 256, &aesKey);

    // Вектор инициализации (IV)
    unsigned char iv[AES_BLOCK_SIZE];
    memset(iv, 0x00, AES_BLOCK_SIZE);

    // Буферы для чтения и записи
    unsigned char inBuffer[AES_BLOCK_SIZE];
    unsigned char outBuffer[AES_BLOCK_SIZE];
    memset(inBuffer, 0, AES_BLOCK_SIZE);

    // Записываем IV в начало файла
    out.write((char*)iv, AES_BLOCK_SIZE);

    // Шифрование данных
    while (in.read((char*)inBuffer, AES_BLOCK_SIZE)) {
        AES_cbc_encrypt(inBuffer, outBuffer, AES_BLOCK_SIZE, &aesKey, iv, AES_ENCRYPT);
        out.write((char*)outBuffer, AES_BLOCK_SIZE);
        memset(inBuffer, 0, AES_BLOCK_SIZE);
    }

    // Обработка последнего блока
    int remaining = in.gcount();
    if (remaining > 0) {
        // PKCS#7 padding
        int padLen = AES_BLOCK_SIZE - remaining;
        memset(inBuffer + remaining, padLen, padLen);
        AES_cbc_encrypt(inBuffer, outBuffer, AES_BLOCK_SIZE, &aesKey, iv, AES_ENCRYPT);
        out.write((char*)outBuffer, AES_BLOCK_SIZE);
    }

    in.close();
    out.close();
    return outputFile;
}

// Функция для дешифрования файла
string decryptFile(const string& inputFile, const vector<unsigned char>& key) {
    // Создаем имя для дешифрованного файла
    string outputFile;

    // Если файл заканчивается на .enc, убираем это расширение
    if (inputFile.length() > 4 && inputFile.substr(inputFile.length() - 4) == ".enc") {
        outputFile = inputFile.substr(0, inputFile.length() - 4) + ".dec";
    }
    else {
        outputFile = inputFile + ".dec";
    }

    ifstream in(inputFile, ios::binary);
    ofstream out(outputFile, ios::binary);

    if (!in) {
        cerr << "Ошибка открытия зашифрованного файла!" << endl;
        return "";
    }
    if (!out) {
        cerr << "Ошибка создания дешифрованного файла!" << endl;
        return "";
    }

    // Инициализация AES
    AES_KEY aesKey;
    AES_set_decrypt_key(key.data(), 256, &aesKey);

    // Читаем IV из начала файла
    unsigned char iv[AES_BLOCK_SIZE];
    in.read((char*)iv, AES_BLOCK_SIZE);

    // Буферы для чтения и записи
    unsigned char inBuffer[AES_BLOCK_SIZE];
    unsigned char outBuffer[AES_BLOCK_SIZE];

    // Дешифрование данных
    while (in.read((char*)inBuffer, AES_BLOCK_SIZE)) {
        AES_cbc_encrypt(inBuffer, outBuffer, AES_BLOCK_SIZE, &aesKey, iv, AES_DECRYPT);

        // Обработка последнего блока (удаление дополнения)
        if (in.peek() == EOF) {
            int padLen = outBuffer[AES_BLOCK_SIZE - 1];
            if (padLen > 0 && padLen <= AES_BLOCK_SIZE) {
                out.write((char*)outBuffer, AES_BLOCK_SIZE - padLen);
            }
            else {
                out.write((char*)outBuffer, AES_BLOCK_SIZE);
            }
        }
        else {
            out.write((char*)outBuffer, AES_BLOCK_SIZE);
        }
    }

    in.close();
    out.close();
    return outputFile;
}

int main() {
    setlocale(LC_ALL, "Russian");
    cout << "AES File Encryptor/Decryptor" << endl;
    cout << "1. Шифровать файл" << endl;
    cout << "2. Дешифровать файл" << endl;
    cout << "Выберите действие: ";

    int choice;
    cin >> choice;
    cin.ignore(); // Очистка буфера

    string inputFile, password;

    cout << "Введите имя файла для обработки: ";
    getline(cin, inputFile);

    cout << "Введите пароль: ";
    getline(cin, password);

    // Генерация ключа из пароля
    vector<unsigned char> key = hashPassword(password);

    string resultFile;
    if (choice == 1) {
        resultFile = encryptFile(inputFile, key);
        if (!resultFile.empty()) {
            cout << "Файл успешно зашифрован. Результат сохранен в: " << resultFile << endl;
        }
        else {
            cerr << "Ошибка при шифровании файла!" << endl;
        }
    }
    else if (choice == 2) {
        resultFile = decryptFile(inputFile, key);
        if (!resultFile.empty()) {
            cout << "Файл успешно дешифрован. Результат сохранен в: " << resultFile << endl;
        }
        else {
            cerr << "Ошибка при дешифровании файла!" << endl;
        }
    }
    else {
        cerr << "Неверный выбор!" << endl;
        return 1;
    }

    return 0;
}