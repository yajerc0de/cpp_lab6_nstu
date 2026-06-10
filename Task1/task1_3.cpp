#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <conio.h>      // для _kbhit(), _getch()
#include <windows.h>    // для Sleep()

using namespace std;

class GameOfLife {
private:
    int rows, cols;
    vector<vector<bool>> grid;
    vector<vector<bool>> nextGrid;
    bool classicRules;

    int countNeighbors(int r, int c) const {
        int cnt = 0;
        for (int dr = -1; dr <= 1; ++dr) {
            for (int dc = -1; dc <= 1; ++dc) {
                if (dr == 0 && dc == 0) continue;
                int nr = (r + dr + rows) % rows;
                int nc = (c + dc + cols) % cols;
                if (grid[nr][nc]) cnt++;
            }
        }
        return cnt;
    }

public:
    GameOfLife(int r, int c, bool classic = true)
        : rows(r), cols(c), classicRules(classic) {
        grid.assign(rows, vector<bool>(cols, false));
        nextGrid.assign(rows, vector<bool>(cols, false));
    }

    void setCell(int r, int c, bool alive) {
        if (r >= 0 && r < rows && c >= 0 && c < cols)
            grid[r][c] = alive;
    }

    void randomFill(double density = 0.3) {
        for (int i = 0; i < rows; ++i)
            for (int j = 0; j < cols; ++j)
                grid[i][j] = ((double)rand() / RAND_MAX) < density;
    }

    // Фигуры
    void addGlider(int topRow, int leftCol) {
        bool pattern[3][3] = { {0,1,0}, {0,0,1}, {1,1,1} };
        for (int i = 0; i < 3; ++i)
            for (int j = 0; j < 3; ++j)
                if (pattern[i][j]) setCell(topRow + i, leftCol + j, true);
    }

    void addLWSS(int topRow, int leftCol) {
        bool pattern[4][5] = {
            {0,1,0,0,1},
            {1,0,0,0,0},
            {1,0,0,0,1},
            {1,1,1,1,0}
        };
        for (int i = 0; i < 4; ++i)
            for (int j = 0; j < 5; ++j)
                if (pattern[i][j]) setCell(topRow + i, leftCol + j, true);
    }

    void addMWSS(int topRow, int leftCol) {
        bool pattern[4][5] = {
            {0,0,1,0,1},
            {1,0,0,0,0},
            {1,0,0,0,1},
            {1,1,1,1,0}
        };
        for (int i = 0; i < 4; ++i)
            for (int j = 0; j < 5; ++j)
                if (pattern[i][j]) setCell(topRow + i, leftCol + j, true);
    }

    void addHWSS(int topRow, int leftCol) {
        bool pattern[4][5] = {
            {0,0,0,1,1},
            {1,0,0,0,0},
            {1,0,0,0,1},
            {1,1,1,1,0}
        };
        for (int i = 0; i < 4; ++i)
            for (int j = 0; j < 5; ++j)
                if (pattern[i][j]) setCell(topRow + i, leftCol + j, true);
    }

    void addQueenBeeShuttle(int topRow, int leftCol) {
        bool pattern[13][5] = {
            {0,0,0,0,0},{0,0,0,0,0},
            {1,1,0,1,1},{1,1,0,1,1},
            {0,0,0,0,0},{0,0,1,0,0},
            {0,0,0,0,0},{0,0,1,0,0},
            {0,0,0,0,0},{1,1,0,1,1},
            {1,1,0,1,1},{0,0,0,0,0},{0,0,0,0,0}
        };
        for (int i = 0; i < 13; ++i)
            for (int j = 0; j < 5; ++j)
                if (pattern[i][j]) setCell(topRow + i, leftCol + j, true);
    }

    void step() {
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                int neighbors = countNeighbors(i, j);
                bool alive = grid[i][j];
                if (classicRules) {
                    if (alive && (neighbors < 2 || neighbors > 3))
                        nextGrid[i][j] = false;
                    else if (!alive && neighbors == 3)
                        nextGrid[i][j] = true;
                    else
                        nextGrid[i][j] = alive;
                } else {
                    // Оригинальные правила
                    if (alive && (neighbors == 2 || neighbors == 3 || neighbors == 4))
                        nextGrid[i][j] = true;
                    else if (!alive && (neighbors == 3 || neighbors == 4))
                        nextGrid[i][j] = true;
                    else
                        nextGrid[i][j] = false;
                }
            }
        }
        swap(grid, nextGrid);
    }

    void display() const {
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j)
                cout << (grid[i][j] ? '*' : ' ');
            cout << '\n';
        }
        cout << flush;
    }

    static void clearScreen() {
        system("cls");
    }

    void run(int delayMs = 100) {
        int gen = 0;
        bool running = true;
        while (running) {
            clearScreen();
            cout << "Поколение " << gen
                 << " | Правила: " << (classicRules ? "классические" : "оригинальные") << "\n";
            display();
            cout << "\nНажмите 'q' для выхода, 's' для паузы/продолжения\n";

            if (_kbhit()) {
                char ch = _getch();
                if (ch == 'q') running = false;
                else if (ch == 's') {
                    cout << "Пауза. Нажмите любую клавишу для продолжения...\n";
                    _getch();
                }
            }

            if (running) {
                step();
                gen++;
                Sleep(delayMs);
            }
        }
    }
};

int main() {
    setlocale(LC_ALL, "Russian");
    srand(static_cast<unsigned>(time(nullptr)));

    cout << "=== Игра Жизнь Конвея (вариант 12) ===\n";
    cout << "Выберите правила:\n1. Классические\n2. Оригинальные (свои)\n> ";
    int rule;
    cin >> rule;
    bool classic = (rule == 1);

    const int ROWS = 40;
    const int COLS = 80;
    GameOfLife game(ROWS, COLS, classic);

    cout << "\nВыберите начальную конфигурацию:\n"
         << "1. Glider\n2. LWSS\n3. MWSS\n4. HWSS\n5. Queen Bee Shuttle\n6. Случайная колония\n> ";
    int fig;
    cin >> fig;

    switch (fig) {
        case 1: game.addGlider(5, 10); break;
        case 2: game.addLWSS(10, 20); break;
        case 3: game.addMWSS(15, 30); break;
        case 4: game.addHWSS(20, 40); break;
        case 5: game.addQueenBeeShuttle(10, 25); break;
        case 6: game.randomFill(0.25); break;
        default: game.addGlider(5, 10);
    }

    game.run(100);

    return 0;
}