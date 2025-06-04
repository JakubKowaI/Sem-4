 #include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int max(int a, int b) {
  if (a > b) {
    return a;
  } else {
    return b;
  }
}

int switchPlayers(int tempPlayer){
  return 3-tempPlayer;
}

     
// Sprawdź, czy istnieje linia `length` symboli `player` w rzędzie (bez przerw)
bool check_line(const int board[5][5], int player, int length) {
    // Sprawdź wiersze, kolumny i przekątne
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            // Sprawdź wiersze (poziomo)
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

            // Sprawdź kolumny (pionowo)
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

            // Sprawdź przekątną (lewo-góra do prawo-dół)
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

            // Sprawdź przekątną (prawo-góra do lewo-dół)
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
            // Upewnij się, że linia nie jest częścią dłuższej sekwencji
            if (match) {
                if ((j == 0 || board[i][j - 1] != player) &&
                    (j + length == 5 || board[i][j + length] != player)) {
                    count++;
                }
            }
        }
    }
    printf("Po liniach poziomych: %d\n",count);
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
    printf("Po liniach pionowych: %d\n",count);

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
    printf("Po przekatnych: %d\n",count);

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

    return count;
}


static bool is_pattern(const int line[4], int player) {
    // XX_X
    if (line[0] == player && line[1] == player && line[2] == 0 && line[3] == player) return true;
    // X_XX
    if (line[0] == player && line[1] == 0 && line[2] == player && line[3] == player) return true;
    return false;
}

// Sprawdza, czy gracz `player` ma linię typu XX-X lub X-XX w dowolnym kierunku
bool has_gap_line(const int board[5][5], int player) {
    // Sprawdź wiersze (poziomo)
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j <= 1; j++) {
            int line[4];
            for (int k = 0; k < 4; k++) line[k] = board[i][j + k];
            if (is_pattern(line, player)) return true;
        }
    }

    // Sprawdź kolumny (pionowo)
    for (int j = 0; j < 5; j++) {
        for (int i = 0; i <= 1; i++) {
            int line[4];
            for (int k = 0; k < 4; k++) line[k] = board[i + k][j];
            if (is_pattern(line, player)) return true;
        }
    }

    // Sprawdź przekątną (lewo-góra do prawo-dół)
    for (int i = 0; i <= 1; i++) {
        for (int j = 0; j <= 1; j++) {
            int line[4];
            for (int k = 0; k < 4; k++) line[k] = board[i + k][j + k];
            if (is_pattern(line, player)) return true;
        }
    }

    // Sprawdź przekątną (prawo-góra do lewo-dół)
    for (int i = 0; i <= 1; i++) {
        for (int j = 4; j >= 3; j--) {
            int line[4];
            for (int k = 0; k < 4; k++) line[k] = board[i + k][j - k];
            if (is_pattern(line, player)) return true;
        }
    }

    return false;
}

// Zlicza liczbę wystąpień linii typu XX_X lub X_XX dla danego gracza
int count_gap_lines(const int board[5][5], int player) {
    int count = 0;

    // Wiersze (poziomo)
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j <= 1; j++) {  // 0,1 → 0-3 i 1-4
            int line[4];
            for (int k = 0; k < 4; k++) line[k] = board[i][j + k];
            if (is_pattern(line, player)) count++;
        }
    }

    // Kolumny (pionowo)
    for (int j = 0; j < 5; j++) {
        for (int i = 0; i <= 1; i++) {
            int line[4];
            for (int k = 0; k < 4; k++) line[k] = board[i + k][j];
            if (is_pattern(line, player)) count++;
        }
    }

    // Przekątne lewo-góra → prawo-dół
    for (int i = 0; i <= 1; i++) {
        for (int j = 0; j <= 1; j++) {
            int line[4];
            for (int k = 0; k < 4; k++) line[k] = board[i + k][j + k];
            if (is_pattern(line, player)) count++;
        }
    }

    // Przekątne prawo-góra → lewo-dół
    for (int i = 0; i <= 1; i++) {
        for (int j = 3; j <= 4; j++) {
            int line[4];
            for (int k = 0; k < 4; k++) line[k] = board[i + k][j - k];
            if (is_pattern(line, player)) count++;
        }
    }

    return count;
}

static bool one_gap_pattern(const int line[3], int player) {
    if(line[0]==player&&line[1]==0&&line[2]==player||
    line[0]==0&&line[1]==player&&line[2]==player||
    line[0]==player&&line[1]==player&&line[2]==0)
    return true;
    return false;
}

int count_lines_with_one_gap(const int board[5][5], int player) {
    int count = 0;

    // Wiersze (poziomo)
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j <= 2; j++) {  // 0,1 → 0-3 i 1-4
            int line[3];
            for (int k = 0; k < 3; k++) line[k] = board[i][j + k];
            if (one_gap_pattern(line, player)) count++;
        }
    }

    // Kolumny (pionowo)
    for (int j = 0; j < 5; j++) {
        for (int i = 0; i <= 2; i++) {
            int line[3];
            for (int k = 0; k < 3; k++) line[k] = board[i + k][j];
            if (one_gap_pattern(line, player)) count++;
        }
    }

    // Przekątne lewo-góra → prawo-dół
    for (int i = 0; i <= 2; i++) {
        for (int j = 0; j <= 2; j++) {
            int line[3];
            for (int k = 0; k < 3; k++) line[k] = board[i + k][j + k];
            if (one_gap_pattern(line, player)) count++;
        }
    }

    // Przekątne prawo-góra → lewo-dół
    for (int i = 0; i <= 2; i++) {
        for (int j = 2; j <= 3; j++) {
            int line[3];
            for (int k = 0; k < 3; k++) line[k] = board[i + k][j - k];
            if (one_gap_pattern(line, player)) count++;
        }
    }

    return count;
}

int aggressive_heuristic(const int board[5][5], int player) {
    // for (int i = 0; i < 5; i++) {
    //     for (int j = 0; j < 5; j++) {
    //         printf("%d",board[i][j]);
    //     }
    //     printf("\n");
    //}
    int value=0;
    value+=5000*count_lines_of_length(board,player,4);
    value-=5000*count_lines_of_length(board,switchPlayers(player),4);
    value+=2500*count_gap_lines(board,player);
    value+=-7000*count_lines_of_length(board,player,3);
    value+=250*count_lines_of_length(board,player,2);
    
    for(int i=0;i<5;i++){
      for(int j=0;j<5;j++){
        if(board[i][j]==player){
          value += 10 * (3 - max(abs(i - 2), abs(j - 2)));
        }
      }
    }
    //printf("Final value: %d\n%d\n",value,tempPlayer);

    return value; 
}

int main(){
    int board[5][5] = {
        {0, 0, 1, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 2, 2, 0, 2},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0}
    };
    //printf("%d\n",aggressive_heuristic(board,2));
    //printf("%d\n",count_lines_of_length(board,2,3));
    //printf("%d\n",count_gap_lines(board,2));
    printf("%d\n",count_lines_with_one_gap(board,2));

    return 0;
}