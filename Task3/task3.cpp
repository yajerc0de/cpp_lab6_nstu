#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>

using namespace std;

// Параметры для варианта 11 (из таблицы)
const double M_val = 0.89;
const double N_val = 0.08;
const double P_val = -1.21;
const double EPS = 1e-3;
const int SIZE = 4;

// Вывод вектора
void printVector(const vector<double>& vec, const string& name) {
    cout << name << ": [";
    for (int i = 0; i < vec.size(); ++i) {
        cout << setw(9) << fixed << setprecision(6) << vec[i];
        if (i < vec.size() - 1) cout << ", ";
    }
    cout << " ]\n";
}

// Прямой метод – LU-разложение
void solveLU(const vector<vector<double>>& A, const vector<double>& b) {
    vector<vector<double>> L(SIZE, vector<double>(SIZE, 0.0));
    vector<vector<double>> U(SIZE, vector<double>(SIZE, 0.0));

    for (int i = 0; i < SIZE; ++i) L[i][i] = 1.0;

    // Вычисление L и U
    for (int i = 0; i < SIZE; ++i) {
        for (int k = i; k < SIZE; ++k) {
            double sum = 0.0;
            for (int j = 0; j < i; ++j) sum += L[i][j] * U[j][k];
            U[i][k] = A[i][k] - sum;
        }
        for (int k = i + 1; k < SIZE; ++k) {
            double sum = 0.0;
            for (int j = 0; j < i; ++j) sum += L[k][j] * U[j][i];
            L[k][i] = (A[k][i] - sum) / U[i][i];
        }
    }

    cout << "\n=== ПРЯМОЙ МЕТОД (LU-разложение) ===\n";
    
    // Решение Ly = b
    vector<double> y(SIZE, 0.0);
    for (int i = 0; i < SIZE; ++i) {
        double sum = 0.0;
        for (int j = 0; j < i; ++j) sum += L[i][j] * y[j];
        y[i] = b[i] - sum;
    }
    printVector(y, "Результат прямого хода (y)");

    // Решение Ux = y
    vector<double> x(SIZE, 0.0);
    for (int i = SIZE - 1; i >= 0; --i) {
        double sum = 0.0;
        for (int j = i + 1; j < SIZE; ++j) sum += U[i][j] * x[j];
        x[i] = (y[i] - sum) / U[i][i];
    }
    printVector(x, "Результат обратного хода (точные корни x)");
}

// Приближённый метод – Зейделя
void solveZeidel(const vector<vector<double>>& A, const vector<double>& b) {
    cout << "\n=== ПРИБЛИЖЁННЫЙ МЕТОД (Зейдель) ===\n";
    
    // Преобразование к виду x = Cx + f
    vector<vector<double>> C(SIZE, vector<double>(SIZE, 0.0));
    vector<double> f(SIZE, 0.0);

    for (int i = 0; i < SIZE; ++i) {
        double diag = A[i][i];
        f[i] = b[i] / diag;
        for (int j = 0; j < SIZE; ++j) {
            if (i == j) C[i][j] = 0.0;
            else C[i][j] = -A[i][j] / diag;
        }
    }

    // Проверка достаточного условия сходимости (норма C < 1)
    double normC = 0.0;
    for (int i = 0; i < SIZE; ++i) {
        double rowSum = 0.0;
        for (int j = 0; j < SIZE; ++j) rowSum += fabs(C[i][j]);
        if (rowSum > normC) normC = rowSum;
    }
    cout << "Норма матрицы C (бесконечная): " << normC << "\n";
    if (normC >= 1.0) {
        cout << "Внимание: достаточное условие сходимости не выполнено (||C|| >= 1).\n";
        cout << "Итерации могут расходиться.\n";
    }

    // Начальное приближение x0 = f
    vector<double> x_curr = f;
    vector<double> x_next(SIZE, 0.0);
    
    // Шапка таблицы
    cout << "\n-------------------------------------------------------------------------------\n";
    cout << "|   N   |    x1     |    x2     |    x3     |    x4     |       eps_n        |\n";
    cout << "-------------------------------------------------------------------------------\n";
    
    // Нулевая итерация
    cout << "| " << setw(5) << 0 << " | ";
    for (double val : x_curr) cout << setw(9) << fixed << setprecision(6) << val << " | ";
    cout << setw(18) << "—" << " |\n";

    int step = 0;
    double eps_n = 1.0;

    while (eps_n > EPS && step < 200) {
        step++;
        x_next = x_curr;
        
        // Одна итерация Зейделя (обновление компонент последовательно)
        for (int i = 0; i < SIZE; ++i) {
            double sum = f[i];
            for (int j = 0; j < SIZE; ++j) {
                sum += C[i][j] * x_next[j];
            }
            x_next[i] = sum;
        }

        // Оценка погрешности (максимальная разность между соседними итерациями)
        eps_n = 0.0;
        for (int i = 0; i < SIZE; ++i) {
            eps_n = max(eps_n, fabs(x_next[i] - x_curr[i]));
        }

        x_curr = x_next;

        cout << "| " << setw(5) << step << " | ";
        for (double val : x_curr) cout << setw(9) << fixed << setprecision(6) << val << " | ";
        cout << setw(18) << scientific << setprecision(6) << eps_n << " |\n";
    }
    cout << "-------------------------------------------------------------------------------\n";

    cout << "\nПриближённые корни с точностью " << EPS << ":\n";
    printVector(x_curr, "x*");
}

int main() {
    // Матрица A для варианта 11 (подстановка M, N, P)
    vector<vector<double>> A = {
        { M_val, -0.04,   0.21, -1.16 },
        {  0.25, -1.23,  N_val, -0.09 },
        { -0.21,  N_val,   0.8, -0.13 },
        {  0.15, -1.31,   0.06,  P_val }
    };
    // Вектор правой части b
    vector<double> b = { -1.24, P_val, 2.56, M_val };

    cout << "Решение СЛАУ для варианта 11:\n";
    cout << "M = " << M_val << ", N = " << N_val << ", P = " << P_val << "\n\n";

    // Прямой метод (LU)
    solveLU(A, b);
    // Приближённый метод (Зейдель)
    solveZeidel(A, b);

    return 0;
}