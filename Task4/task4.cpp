#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>

using namespace std;
enum class Choi {
    DEMO = 1,
    GEN         
};
int N = 4, M = 3;
int total_bits;

// Преобразование координат (r, c) в индекс бита 
int getBitIndex(int r, int c) {
    return r * M + c;
}

// Проверка на победу 
bool isTarget(int mask) {
    int half = N / 2;
    int top_val = mask & 1; 
    int bottom_val = (mask >> (total_bits - 1)) & 1;

    if (top_val == bottom_val) return false;

    for (int r = 0; r < N; ++r) {
        int expected = (r < half) ? top_val : bottom_val;
        for (int c = 0; c < M; ++c) {
            int current_bit = (mask >> getBitIndex(r, c)) & 1;
            if (current_bit != expected) return false;
        }
    }
    return true;
}

// Красивый вывод доски 
void printBoardFromMask(int mask) {
    for (int r = 0; r < N; ++r) {
        cout << "| ";
        for (int c = 0; c < M; ++c) {
            cout << ((mask >> getBitIndex(r, c)) & 1) << " ";
        }
        cout << "|\n";
    }
}

int main() {
    setlocale(LC_ALL, "Russian");
    srand(time(0));

    cout << "Выберите режим:\n1. Пример с картинки (4x3)\n2. Случайная генерация\nВаш выбор: ";
    int choice;
    if (!(cin >> choice)) choice = 1;
    Choi userChoice = static_cast<Choi>(choice);
    if (userChoice == Choi::DEMO) {
        N = 4; M = 3;
    } else {
        cout << "Введите N и M (так, чтобы N * M не превышало 22 для экономии памяти): ";
        cin >> N >> M;
    }

    total_bits = N * M;
    int max_states = 1 << total_bits; // Общее количество возможных состояний 

    int start_mask = 0;

    if (userChoice == Choi::DEMO) {
        int example_matrix[4][3] = {
            {1, 0, 1},
            {0, 1, 0},
            {1, 1, 0},
            {0, 0, 1}
        };
        for (int r = 0; r < N; ++r) {
            for (int c = 0; c < M; ++c) {
                if (example_matrix[r][c] == 1) {
                    start_mask |= (1 << getBitIndex(r, c));
                }
            }
        }
    } else {
        for (int r = 0; r < N; ++r) {
            for (int c = 0; c < M; ++c) {
                if (rand() % 2 == 1) {
                    start_mask |= (1 << getBitIndex(r, c));
                }
            }
        }
        cout << "\nСгенерирована случайная доска:\n";
        printBoardFromMask(start_mask);
    }

    cout << "\nМгновенный поиск решения через быстрый BFS...\n";

    // Выделяем память под массивы 
    vector<int> parent(max_states, -1); // для запоминания от какой доски получилась доска
    vector<int> q(max_states); //очередь из которой будем доставать и ксорить доски
    int head = 0, tail = 0;

    // Помещаем стартовое состояние в очередь
    q[tail++] = start_mask;
    parent[start_mask] = start_mask; // помечаем как посещенную (само на себя)

    int target_mask = -1;


    while (head < tail) {
        int curr = q[head++];

        if (isTarget(curr)) {
            target_mask = curr;
            break;
        }

        // Генерируем все ходы с этой вариации доски
        for (int r = 0; r < N; ++r) {
            for (int c = 0; c < M; ++c) {
                // Проверяем вправо
                if (c + 1 < M) {
                    int next_mask = curr ^ (1 << getBitIndex(r, c)) ^ (1 << getBitIndex(r, c + 1));
                    if (parent[next_mask] == -1) { // если еще не посещали 
                        parent[next_mask] = curr;  // запоминаем, что до этого была такая-то доска
                        q[tail++] = next_mask;     // И добавляем эту вариацию в очередь для проверки
                    }
                }
                // Проверяем вниз
                if (r + 1 < N) {
                    int next_mask = curr ^ (1 << getBitIndex(r, c)) ^ (1 << getBitIndex(r + 1, c));
                    if (parent[next_mask] == -1) {
                        parent[next_mask] = curr;
                        q[tail++] = next_mask;
                    }
                }
            }
        }
    }

    // Если мы вышли из цикла и не нашли мишень
    if (target_mask == -1) {
        cout << "Решение НЕ существует математически! (Конфигурация тупиковая).\n";
        return 0;
    }

    // Восстанавливаем точный путь от финиша к старту
    vector<int> path;
    int curr = target_mask;
    while (curr != start_mask) {
        path.push_back(curr);
        curr = parent[curr];
    }
    path.push_back(start_mask);

    cout << "\n=== НАЧАЛО ПОШАГОВОГО ВЫВОДА ===\n\n";
    int steps = 0;
    for (int i = path.size() - 1; i >= 0; --i) {
        cout << "Шаг " << steps++ << ":\n";
        printBoardFromMask(path[i]);
        cout << "\n";
    }

    cout << "Минимальное количество ходов: " << path.size() - 1 << endl;
    return 0;
}