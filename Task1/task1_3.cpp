#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <string>
#include <thread>
#include <chrono>

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

    void addPattern(const vector<string>& pattern, int topRow, int leftCol) {
        for (int i = 0; i < (int)pattern.size(); ++i)
            for (int j = 0; j < (int)pattern[i].size(); ++j)
                if (pattern[i][j] == 'O') setCell(topRow + i, leftCol + j, true);
    }

    // Фигуры
    void addGlider(int topRow, int leftCol) {
        addPattern({ ".O.",
                     "..O",
                     "OOO" }, topRow, leftCol);
    }

    void addLWSS(int topRow, int leftCol) {
        addPattern({ ".O..O",
                     "O....",
                     "O...O",
                     "OOOO." }, topRow, leftCol);
    }

    void addMWSS(int topRow, int leftCol) {
        addPattern({ "...O..",
                     ".O...O",
                     "O.....",
                     "O....O",
                     "OOOOO." }, topRow, leftCol);
    }

    void addHWSS(int topRow, int leftCol) {
        addPattern({ "...OO..",
                     ".O....O",
                     "O......",
                     "O.....O",
                     "OOOOOO." }, topRow, leftCol);
    }

    void addQueenBeeShuttle(int topRow, int leftCol) {
        addPattern({ ".........O............",
                     ".......O.O............",
                     "......O.O.............",
                     "OO...O..O...........OO",
                     "OO....O.O...........OO",
                     ".......O.O............",
                     ".........O............" }, topRow, leftCol);
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
#ifdef _WIN32
        system("cls");
#else
        cout << "\033[2J\033[H";
#endif
    }

    void run(int generations, int delayMs) {
        for (int gen = 0; gen <= generations; ++gen) {
            clearScreen();
            cout << "Поколение " << gen << " из " << generations
                 << " | Правила: " << (classicRules ? "классические" : "оригинальные") << "\n";
            display();
            step();
            this_thread::sleep_for(chrono::milliseconds(delayMs));
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

    cout << "\nСколько поколений показать: ";
    int generations;
    cin >> generations;
    if (generations < 1) generations = 100;

    game.run(generations, 100);

    return 0;
}