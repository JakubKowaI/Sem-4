#include <stdbool.h>
#include <stdio.h>

#define SIZE 5

// Sprawdza, czy w tablicy `line` o długości 4 występuje wzorzec XX-X lub X-XX
bool is_pattern(const char line[4], char player) {
    // Sprawdź XX-X (np. X X _ X)
    bool pattern1 = (line[0] == player && line[1] == player && line[3] == player && line[2] == ' ');
    // Sprawdź X-XX (np. X _ X X)
    bool pattern2 = (line[0] == player && line[2] == player && line[3] == player && line[1] == ' ');
    return pattern1 || pattern2;
}

// Sprawdza, czy gracz `player` ma linię typu XX-X lub X-XX w dowolnym kierunku
bool has_gap_line(const char board[SIZE][SIZE], char player) {
    // Sprawdź wiersze (poziomo)
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j <= SIZE - 4; j++) {
            char line[4];
            for (int k = 0; k < 4; k++) line[k] = board[i][j + k];
            if (is_pattern(line, player)) return true;
        }
    }

    // Sprawdź kolumny (pionowo)
    for (int j = 0; j < SIZE; j++) {
        for (int i = 0; i <= SIZE - 4; i++) {
            char line[4];
            for (int k = 0; k < 4; k++) line[k] = board[i + k][j];
            if (is_pattern(line, player)) return true;
        }
    }

    // Sprawdź przekątną (lewo-góra do prawo-dół)
    for (int i = 0; i <= SIZE - 4; i++) {
        for (int j = 0; j <= SIZE - 4; j++) {
            char line[4];
            for (int k = 0; k < 4; k++) line[k] = board[i + k][j + k];
            if (is_pattern(line, player)) return true;
        }
    }

    // Sprawdź przekątną (prawo-góra do lewo-dół)
    for (int i = 0; i <= SIZE - 4; i++) {
        for (int j = SIZE - 1; j >= 3; j--) {
            char line[4];
            for (int k = 0; k < 4; k++) line[k] = board[i + k][j - k];
            if (is_pattern(line, player)) return true;
        }
    }

    return false;
}

// Przykład użycia
int main() {
    char board[SIZE][SIZE] = {
        {'X', 'X', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', 'X'},
        {' ', ' ', ' ', 'X', 'X'},
        {' ', ' ', ' ', ' ', 'X'},
        {'X', ' ', 'X', ' ', 'X'}
    };

    printf("Czy X ma linię typu XX-X lub X-XX? %s\n", 
           has_gap_line(board, 'X') ? "Tak" : "Nie");

    return 0;
}