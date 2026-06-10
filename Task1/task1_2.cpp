#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <ctime>
#include <map>
#include <vector>
#include <algorithm>

using namespace std;

int main() {
    int M, N;
    cout << "Задание 2. Матрица M x N (M > 5, N > 5).\nВведите M и N: ";
    cin >> M >> N;
    if (M <= 5) M = 6;
    if (N <= 5) N = 6;

    // Выделение динамической матрицы
    int** matrix = new int*[M];
    for (int i = 0; i < M; ++i)
        matrix[i] = new int[N];

    // Заполнение случайными [100,150]
    srand(static_cast<unsigned>(time(nullptr)));
    for (int i = 0; i < M; ++i)
        for (int j = 0; j < N; ++j)
            matrix[i][j] = rand() % 51 + 100;

    // Вывод исходной матрицы
    cout << "\nИсходная матрица " << M << "x" << N << ":\n";
    for (int i = 0; i < M; ++i) {
        for (int j = 0; j < N; ++j)
            cout << setw(5) << matrix[i][j] << " ";
        cout << endl;
    }

    // Подсчёт вхождений каждого элемента (через map)
    map<int, int> freq;
    for (int i = 0; i < M; ++i)
        for (int j = 0; j < N; ++j)
            freq[matrix[i][j]]++;

    // Отбор только повторяющихся (частота > 1)
    vector<pair<int, int>> repeating;
    for (auto& p : freq)
        if (p.second > 1)
            repeating.push_back(p);

    // Сортировка по возрастанию значений элементов
    sort(repeating.begin(), repeating.end(),
         [](const pair<int,int>& a, const pair<int,int>& b) { return a.first < b.first; });

    // Вывод таблицы
    cout << "\nТаблица (Элемент | Количество вхождений) для повторяющихся:\n";
    if (repeating.empty())
        cout << "Нет повторяющихся элементов.\n";
    else {
        for (auto& p : repeating)
            cout << setw(10) << p.first << " | " << p.second << endl;
    }

    // Поиск элемента с наибольшим количеством вхождений
    if (!repeating.empty()) {
        auto maxElem = *max_element(repeating.begin(), repeating.end(),
                                    [](const pair<int,int>& a, const pair<int,int>& b) {
                                        return a.second < b.second;
                                    });
        cout << "\nЭлемент, встречающийся наибольшее число раз: " << maxElem.first
             << " (встречается " << maxElem.second << " раз)\n";
    }

    // Освобождение памяти
    for (int i = 0; i < M; ++i)
        delete[] matrix[i];
    delete[] matrix;

    return 0;
}