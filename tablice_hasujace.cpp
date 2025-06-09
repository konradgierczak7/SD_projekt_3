#include <iostream>
#include <iomanip>
#include <random>
#include <string>
#include <cmath>
#include <chrono>
#include <cstdlib>

using namespace std;
using namespace chrono;

const int EMPTY = -1;
const int MAX_SIZE = 51000;

using HashFunc = int(*)(int, int);

int moduloHash(int key, int m) {
    return key % m;
}

int multiplicationHash(int key, int m) {
    double A = 0.6180339887;
    double x = key * A;
    return (int)((x - (int)x) * m);
}

int digitSumHash(int key, int m) {
    int sum = 0, tmp = abs(key);
    while (tmp > 0) {
        sum += tmp % 10;
        tmp /= 10;
    }
    return sum % m;
}

class HashTable {
    int table[MAX_SIZE];
    int size;
    HashFunc hashFunc;

public:
    HashTable(int m, HashFunc f) : size(m), hashFunc(f) {
        for (int i = 0; i < size; ++i)
            table[i] = EMPTY;
    }

    bool insert(int key, int& probes) {
        int index = hashFunc(key, size);
        probes = 0;
        for (int i = 0; i < size; ++i) {
            int probeIndex = (index + i) % size;
            probes++;
            if (table[probeIndex] == EMPTY) {
                table[probeIndex] = key;
                return true;
            }
        }
        return false;
    }

    bool remove(int key, int& probes) {
        int index = hashFunc(key, size);
        probes = 0;
        for (int i = 0; i < size; ++i) {
            int probeIndex = (index + i) % size;
            probes++;
            if (table[probeIndex] == key) {
                table[probeIndex] = EMPTY;
                return true;
            }
            if (table[probeIndex] == EMPTY)
                return false;
        }
        return false;
    }
};

void testFunction(HashFunc h, const string& name, int data[], int dataSize, int m) {
    long long totalInsertTime_ns = 0;
    long long totalRemoveTime_ns = 0;
    long long totalInsertProbes = 0;
    long long totalRemoveProbes = 0;
    const int repetitions = 1000;

    for (int j = 0; j < repetitions; j++) {
        HashTable ht(m, h);

        auto start = high_resolution_clock::now();
        for (int i = 0; i < dataSize; ++i) {
            int probes;
            ht.insert(data[i], probes);
            totalInsertProbes += probes;
        }
        auto mid = high_resolution_clock::now();

        for (int i = 0; i < dataSize; ++i) {
            int probes;
            ht.remove(data[i], probes);
            totalRemoveProbes += probes;
        }
        auto end = high_resolution_clock::now();

        totalInsertTime_ns += duration_cast<nanoseconds>(mid - start).count();
        totalRemoveTime_ns += duration_cast<nanoseconds>(end - mid).count();
    }

    double avgInsertProbes = (double)totalInsertProbes / (repetitions * dataSize);
    double avgRemoveProbes = (double)totalRemoveProbes / (repetitions * dataSize);
    double avgInsertTime_us = (double)totalInsertTime_ns / repetitions / 1000.0;
    double avgRemoveTime_us = (double)totalRemoveTime_ns / repetitions / 1000.0;

    cout << "\nFunkcja: " << name << "\n";
    cout << "\tŚrednia liczba prób przy wstawianiu: " << avgInsertProbes << "\n";
    cout << "\tŚrednia liczba prób przy usuwaniu: " << avgRemoveProbes << "\n";
    cout << "\tŚredni czas operacji (wstawienie): " << avgInsertTime_us << " us\n";
    cout << "\tŚredni czas operacji (usuwanie):   " << avgRemoveTime_us << " us\n";
}

int main() {
    int tableSize = 50000;
    int dataSize = 25000;
    int testData[MAX_SIZE];

    cout << fixed << setprecision(4);
    cout << "=======================================================\n";
    cout << "Rozmiar tablicy: " << tableSize << ", Liczba danych: " << dataSize << "\n";
    cout << "Współczynnik wypełnienia (alpha): " << (double)dataSize / tableSize << "\n";
    cout << "=======================================================\n";

    cout << "\n--- MODULO ---";

    for (int i = 0; i < dataSize; ++i)
        testData[i] = i;
    cout << "\nPrzypadek optymistyczny:";
    testFunction(moduloHash, "Modulo", testData, dataSize, tableSize);

    mt19937 rng(42);
    uniform_int_distribution<int> dist(0, 10000);
    for (int i = 0; i < dataSize; ++i)
        testData[i] = dist(rng);
    cout << "\nPrzypadek średni:";
    testFunction(moduloHash, "Modulo", testData, dataSize, tableSize);

    for (int i = 0; i < dataSize; ++i)
        testData[i] = i * tableSize;
    cout << "\nPrzypadek pesymistyczny:";
    testFunction(moduloHash, "Modulo", testData, dataSize, tableSize);


    cout << "\n\n--- MULTIPLICATION ---";

    for (int i = 0; i < dataSize; ++i)
        testData[i] = i;
    cout << "\nPrzypadek optymistyczny:";
    testFunction(multiplicationHash, "Multiplication", testData, dataSize, tableSize);

    for (int i = 0; i < dataSize; ++i)
        testData[i] = dist(rng);
    cout << "\nPrzypadek średni:";
    testFunction(multiplicationHash, "Multiplication", testData, dataSize, tableSize);

    for (int i = 0; i < dataSize; ++i)
        testData[i] = i * 111;
    cout << "\nPrzypadek pesymistyczny:";
    testFunction(multiplicationHash, "Multiplication", testData, dataSize, tableSize);


    cout << "\n\n--- DIGIT SUM ---";

    for (int i = 0; i < dataSize; ++i)
        testData[i] = i;
    cout << "\nPrzypadek optymistyczny:";
    testFunction(digitSumHash, "DigitSum", testData, dataSize, tableSize);

    for (int i = 0; i < dataSize; ++i)
        testData[i] = dist(rng);
    cout << "\nPrzypadek średni:";
    testFunction(digitSumHash, "DigitSum", testData, dataSize, tableSize);

    for (int i = 0; i < dataSize; ++i)
        testData[i] = 111;
    cout << "\nPrzypadek pesymistyczny:";
    testFunction(digitSumHash, "DigitSum", testData, dataSize, tableSize);

    return 0;
}