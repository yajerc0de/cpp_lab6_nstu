#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <ctime>
#include <vector>

using namespace std;

int main() {
    int N;
    cout << "Задание 1. Квадратная матрица N x N (N > 5).\nВведите N: ";
    cin >> N;
    if (N <= 5) {
        cout << "Ошибка: N должно быть > 5. Установлено N = 6.\n";
        N = 6;
    }

    // Выделение динамической матрицы
    int** matrix = new int*[N];
    for (int i = 0; i < N; ++i)
        matrix[i] = new int[N];

    // Заполнение случайными числами [-60, 100]
    srand(static_cast<unsigned>(time(nullptr)));
    for (int i = 0; i < N; ++i)
        for (int j = 0; j < N; ++j)
            matrix[i][j] = rand() % 161 - 60;

    // Вывод исходной матрицы
    cout << "\nИсходная матрица " << N << "x" << N << ":\n";
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j)
            cout << setw(5) << matrix[i][j] << " ";
        cout << endl;
    }

    // Подсчёт положительных элементов по строкам
    vector<pair<int, int>> rowPosCount; // (номер строки, количество положительных)
    for (int i = 0; i < N; ++i) {
        int count = 0;
        for (int j = 0; j < N; ++j)
            if (matrix[i][j] > 0) ++count;
        rowPosCount.push_back({i, count});
    }

    // Вывод таблицы
    cout << "\nТаблица (Номер строки | Количество положительных элементов):\n";
    for (auto& p : rowPosCount)
        cout << setw(10) << p.first << " | " << p.second << endl;

    // Поиск строки с наибольшим количеством (первой по порядку)
    int bestRow = 0, bestCount = rowPosCount[0].second;
    for (auto& p : rowPosCount) {
        if (p.second > bestCount) {
            bestCount = p.second;
            bestRow = p.first;
        }
    }

    // Создание одномерного динамического массива элементов этой строки
    int* rowArray = new int[N];
    for (int j = 0; j < N; ++j)
        rowArray[j] = matrix[bestRow][j];

    // Вывод результата
    cout << "\nСтрока с наибольшим количеством положительных элементов: №" << bestRow
         << ", количество: " << bestCount << "\nЭлементы строки: ";
    for (int j = 0; j < N; ++j)
        cout << rowArray[j] << " ";
    cout << endl;

    // Освобождение памяти
    for (int i = 0; i < N; ++i)
        delete[] matrix[i];
    delete[] matrix;
    delete[] rowArray;

    return 0;
}