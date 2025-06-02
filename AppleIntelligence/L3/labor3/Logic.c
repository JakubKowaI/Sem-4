#include "Logic.h"
#include "board.h"

int max(int a, int b) {
  if (a > b) {
    return a;
  } else {
    return b;
  }
}

bool checkWinState(int tempBoard[5][5], int player) {
  bool w = false;
  for (int i = 0; i < 28; i++)
     if ( (tempBoard[win[i][0][0]][win[i][0][1]] == player) &&
          (tempBoard[win[i][1][0]][win[i][1][1]] == player) &&
          (tempBoard[win[i][2][0]][win[i][2][1]] == player) &&
          (tempBoard[win[i][3][0]][win[i][3][1]] == player) )
      w = true;
  return w;
}

int switchPlayers(int tempPlayer){
  return 3-tempPlayer;
}

bool check_line(const int board[5][5], int player, int length) {
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            // Poziome linie
            if (j + length <= 5) {
                bool win = true;
                for (int k = 0; k < length; k++) {
                    if (board[i][j + k] != player) {
                        win = false;
                        break;
                    }
                }
                if (win) return true;
            }

            // Pionowe linie
            if (i + length <= 5) {
                bool win = true;
                for (int k = 0; k < length; k++) {
                    if (board[i + k][j] != player) {
                        win = false;
                        break;
                    }
                }
                if (win) return true;
            }

            // Przekątne (lewo-góra do prawo-dół)
            if (i + length <= 5 && j + length <= 5) {
                bool win = true;
                for (int k = 0; k < length; k++) {
                    if (board[i + k][j + k] != player) {
                        win = false;
                        break;
                    }
                }
                if (win) return true;
            }

            // Przekątne (prawo-góra do lewo-dół)
            if (i + length <= 5 && j - length + 1 >= 0) {
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

static bool is_pattern(const int line[4], int player) {
    // XX_X
    if (line[0] == player && line[1] == player && line[2] == 0 && line[3] == player) return true;
    // X_XX
    if (line[0] == player && line[1] == 0 && line[2] == player && line[3] == player) return true;
    return false;
}

static bool interrupted_pattern(const int line[4], int player) {
    int opponent=switchPlayers(player);
    // XX_X
    if (line[0] == player && line[1] == player && line[2] == opponent && line[3] == player) return true;
    // X_XX
    if (line[0] == player && line[1] == opponent && line[2] == player && line[3] == player) return true;
    return false;
}

bool has_gap_line(const int board[5][5], int player) {
    // Poziome linie
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j <= 1; j++) {
            int line[4];
            for (int k = 0; k < 4; k++) line[k] = board[i][j + k];
            if (is_pattern(line, player)) return true;
        }
    }

    // Poionowe linie
    for (int j = 0; j < 5; j++) {
        for (int i = 0; i <= 1; i++) {
            int line[4];
            for (int k = 0; k < 4; k++) line[k] = board[i + k][j];
            if (is_pattern(line, player)) return true;
        }
    }

    // Przekątne (lewo-góra do prawo-dół)
    for (int i = 0; i <= 1; i++) {
        for (int j = 0; j <= 1; j++) {
            int line[4];
            for (int k = 0; k < 4; k++) line[k] = board[i + k][j + k];
            if (is_pattern(line, player)) return true;
        }
    }

    // Przekątne (prawo-góra do lewo-dół)
    for (int i = 0; i <= 1; i++) {
        for (int j = 4; j >= 3; j--) {
            int line[4];
            for (int k = 0; k < 4; k++) line[k] = board[i + k][j - k];
            if (is_pattern(line, player)) return true;
        }
    }

    return false;
}

int count_lines_of_length(const int board[5][5], int player, int length) {
    int count = 0;

    // Poziome linie
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j <= 5 - length; j++) {
            bool match = true;
            for (int k = 0; k < length; k++) {
                if (board[i][j + k] != player) {
                    match = false;
                    break;
                }
            }
            if (match) {
                if ((j == 0 || board[i][j - 1] != player) &&
                    (j + length == 5 || board[i][j + length] != player)) {
                    count++;
                }
            }
        }
    }
    //printf("Po liniach poziomych: %d\n",count);

    // Pionowe linie
    for (int i = 0; i <= 5 - length; i++) {
        for (int j = 0; j < 5; j++) {
            bool match = true;
            for (int k = 0; k < length; k++) {
                if (board[i + k][j] != player) {
                    match = false;
                    break;
                }
            }
            if (match) {
                if ((i == 0 || board[i - 1][j] != player) &&
                    (i + length == 5 || board[i + length][j] != player)) {
                    count++;
                }
            }
        }
    }
    //printf("Po liniach pionowych: %d\n",count);

    // Przekątne lewa-góra -> prawa-dół
    for (int i = 0; i <= 5 - length; i++) {
        for (int j = 0; j <= 5 - length; j++) {
            bool match = true;
            for (int k = 0; k < length; k++) {
                if (board[i + k][j + k] != player) {
                    match = false;
                    break;
                }
            }
            if (match) {
                if ((i == 0 || j == 0 || board[i - 1][j - 1] != player) &&
                    (i + length == 5 || j + length == 5 || board[i + length][j + length] != player)) {
                    count++;
                }
            }
        }
    }
    

    // Przekątne prawa-góra -> lewa-dół
    for (int i = 0; i <= 5 - length; i++) {
        for (int j = length - 1; j < 5; j++) {
            bool match = true;
            for (int k = 0; k < length; k++) {
                if (board[i + k][j - k] != player) {
                    match = false;
                    break;
                }
            }
            if (match) {
                if ((i == 0 || j == 4 || board[i - 1][j + 1] != player) &&
                    (i + length == 5 || j - length + 1 == 0 || board[i + length][j - length] != player)) {
                    count++;
                }
            }
        }
    }
    //printf("Po przekatnych: %d\n",count);

    return count;
}

int count_gap_lines(const int board[5][5], int player) {
    int count = 0;

    // Poziome linie
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j <= 1; j++) {  // 0,1 → 0-3 i 1-4
            int line[4];
            for (int k = 0; k < 4; k++) line[k] = board[i][j + k];
            if (is_pattern(line, player)) count++;
        }
    }

    // Pionowe linie
    for (int j = 0; j < 5; j++) {
        for (int i = 0; i <= 1; i++) {
            int line[4];
            for (int k = 0; k < 4; k++) line[k] = board[i + k][j];
            if (is_pattern(line, player)) count++;
        }
    }

    // Przekątne lewo-góra -> prawo-dół
    for (int i = 0; i <= 1; i++) {
        for (int j = 0; j <= 1; j++) {
            int line[4];
            for (int k = 0; k < 4; k++) line[k] = board[i + k][j + k];
            if (is_pattern(line, player)) count++;
        }
    }

    // Przekątne prawo-góra -> lewo-dół
    for (int i = 0; i <= 1; i++) {
        for (int j = 3; j <= 4; j++) {
            int line[4];
            for (int k = 0; k < 4; k++) line[k] = board[i + k][j - k];
            if (is_pattern(line, player)) count++;
        }
    }

    return count;
}

int count_interruped_lines(const int board[5][5], int player) {
    int count = 0;

    // Poziome linie
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j <= 1; j++) {  // 0,1 → 0-3 i 1-4
            int line[4];
            for (int k = 0; k < 4; k++) line[k] = board[i][j + k];
            if (interrupted_pattern(line, player)) count++;
        }
    }

    // Pionowe linie
    for (int j = 0; j < 5; j++) {
        for (int i = 0; i <= 1; i++) {
            int line[4];
            for (int k = 0; k < 4; k++) line[k] = board[i + k][j];
            if (interrupted_pattern(line, player)) count++;
        }
    }

    // Przekątne lewo-góra -> prawo-dół
    for (int i = 0; i <= 1; i++) {
        for (int j = 0; j <= 1; j++) {
            int line[4];
            for (int k = 0; k < 4; k++) line[k] = board[i + k][j + k];
            if (interrupted_pattern(line, player)) count++;
        }
    }

    // Przekątne prawo-góra -> lewo-dół
    for (int i = 0; i <= 1; i++) {
        for (int j = 3; j <= 4; j++) {
            int line[4];
            for (int k = 0; k < 4; k++) line[k] = board[i + k][j - k];
            if (interrupted_pattern(line, player)) count++;
        }
    }

    return count;
}