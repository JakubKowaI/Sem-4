#include <stdbool.h>
#include <stdio.h>

#define SIZE 5

// Sprawdź, czy istnieje linia `length` symboli `player` w rzędzie (bez przerw)
bool check_line(const char board[SIZE][SIZE], char player, int length) {
    // Sprawdź wiersze, kolumny i przekątne
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            // Sprawdź wiersze (poziomo)
            if (j + length <= SIZE) {
                bool win = true;
                for (int k = 0; k < length; k++) {
                    if (board[i][j + k] != player) {
                        win = false;
                        break;
                    }
                }
                if (win) return true;
            }

            // Sprawdź kolumny (pionowo)
            if (i + length <= SIZE) {
                bool win = true;
                for (int k = 0; k < length; k++) {
                    if (board[i + k][j] != player) {
                        win = false;
                        break;
                    }
                }
                if (win) return true;
            }

            // Sprawdź przekątną (lewo-góra do prawo-dół)
            if (i + length <= SIZE && j + length <= SIZE) {
                bool win = true;
                for (int k = 0; k < length; k++) {
                    if (board[i + k][j + k] != player) {
                        win = false;
                        break;
                    }
                }
                if (win) return true;
            }

            // Sprawdź przekątną (prawo-góra do lewo-dół)
            if (i + length <= SIZE && j - length + 1 >= 0) {
                bool win = true;
                for (int k = 0; k < length; k++) {
                    if (board[i + k][j - k] != player) {
                        win = false;
                        break;
                    }
                }
                if (win) return true;
            }
        }
    }
    return false;
}

// Sprawdź, czy gracz `player` ma linię 4 (wygrywa)
bool has_winning_line(const char board[SIZE][SIZE], char player) {
    return check_line(board, player, 4);
}

// Sprawdź, czy gracz `player` ma linię 3 (przegrywa, o ile nie ma też linii 4)
bool has_losing_line(const char board[SIZE][SIZE], char player) {
    return check_line(board, player, 3) && !check_line(board, player, 4);
}

// Przykład użycia
int main() {
    char board[SIZE][SIZE] = {
        {' ', 'X', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', 'X', ' '},
        {' ', 'X', ' ', ' ', ' '},
        {' ', ' ', '0', '0', '0'}
    };

    printf("Czy X ma linię 4? %s\n", check_line(board, '0', 4) ? "Tak" : "Nie");
    printf("Czy X ma linię 3? %s\n", check_line(board, '0', 3) ? "Tak" : "Nie");
    printf("Czy X ma linię 2? %s\n", check_line(board, '0', 2) ? "Tak" : "Nie");

    return 0;
}