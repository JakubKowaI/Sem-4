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

bool is_pattern(const int line[4], int player) {
    // XX_X
    if (line[0] == player && line[1] == player && line[2] == 0 && line[3] == player) return true;
    // X_XX
    if (line[0] == player && line[1] == 0 && line[2] == player && line[3] == player) return true;
    return false;
}

bool is_three_pattern(const int line[4], int player) {
    // -XXX
    if (line[0] != player && line[1] == player && line[2] == player && line[3] == player) return true;
    // XXX-
    if (line[0] == player && line[1] == player && line[2] == player && line[3] != player) return true;
    return false;
}

bool is_win_pattern(const int line[4], int player) {
    // XXXX
    if (line[0] == player && line[1] == player && line[2] == player && line[3] == player) return true;
    return false;
}

bool interrupted_pattern(const int line[4], int player) {
    int opponent=switchPlayers(player);
    // XXOX
    if (line[0] == player && line[1] == player && line[2] == opponent && line[3] == player) return true;
    // XOXX
    if (line[0] == player && line[1] == opponent && line[2] == player && line[3] == player) return true;
    return false;
}

bool is_empty_pattern(const int line[4], int player) {
    // X__X
    if (line[0] == player && line[1] == 0 && line[2] == 0 && line[3] == player) return true;
    return false;
}

bool is_free_pattern(const int line[4], int player) {
    // _XX_
    if (line[0] == 0 && line[1] == player && line[2] == player && line[3] == 0) return true;
    return false;
}

bool is_one_side_blocked_pattern(const int line[4], int player) {
    int opponent=switchPlayers(player);
    // OXX_
    if (line[0] == opponent && line[1] == player && line[2] == player && line[3] == 0) return true;
    // _XXO
    if (line[0] == 0 && line[1] == player && line[2] == player && line[3] == opponent) return true;
    return false;
}

bool is_checkered_pattern(const int line[4], int player) {
    // X_X_
    if (line[0] == player && line[1] == 0 && line[2] == player && line[3] == 0) return true;
    // _X_X
    if (line[0] == 0 && line[1] == player && line[2] == 0 && line[3] == player) return true;
    return false;
}

bool is_two_on_the_side_pattern(const int line[4], int player) {
    // XX__
    if (line[0] == player && line[1] == player && line[2] == 0 && line[3] == 0) return true;
    // __XX
    if (line[0] == 0 && line[1] == 0 && line[2] == player && line[3] == player) return true;
    return false;
}

bool is_blind_two_on_the_side_pattern(const int line[4], int player) {
    int opponent=switchPlayers(player);
    // XX__
    if (line[0] == player && line[1] == player && line[2] == 0 && line[3] == opponent) return true;
    // __XX
    if (line[0] == opponent && line[1] == 0 && line[2] == player && line[3] == player) return true;
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
        for (int j = 0; j <= 1; j++) {
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

bool one_gap_pattern(const int line[3], int player) {
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
        for (int j = 0; j <= 2; j++) {
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

int evaluate_line(int a, int b, int c, int d, int player) {
    int score=0;
    int opponent = switchPlayers(player);

    int line[4] = {a, b, c, d};

    for(int i=0;i<2;i++){
        if(line[i]==player&&line[i+1]==player)score+=250;
    }
    for(int i=0;i<2;i++){
        int t[3]={line[i],line[i+1],line[i+2]};
        if(one_gap_pattern(t,player))score+=2500;
    }
    for(int i=0;i<2;i++){
        int t[3]={line[i],line[i+1],line[i+2]};
        if(line[i]==player&&line[i+1]==player&&line[i+2]==player)score-=50000;
    }
    if(a==player&&b==player&&c==player&&d==player)score+=1000000;
    if(a==opponent&&b==opponent&&c==opponent&&d==opponent)score-=1000000;
    
    if(is_pattern(line,player))score+=40000;
    if(interrupted_pattern(line,player))score-=10000;
    if(interrupted_pattern(line,opponent))score+=20000;
    return score;
}

int test(const int board[5][5],int player){
    int score=0;
    // Wiersze
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j <= 1; j++) {
            score += evaluate_line(board[i][j], board[i][j+1], board[i][j+2], board[i][j+3], player);
        }
    }

    // Kolumny
    for (int i = 0; i <= 1; i++) {
        for (int j = 0; j < 5; j++) {
            score += evaluate_line(board[i][j], board[i+1][j], board[i+2][j], board[i+3][j], player);
        }
    }

    // Przekątne 
    for (int i = 0; i <= 2; i++) {
        for (int j = 0; j <= 2; j++) {
            score += evaluate_line(board[i][j], board[i+1][j+1], board[i+2][j+2], board[i+3][j+3], player);
        }
    }

    // Przekątne 
    for (int i = 0; i <= 2; i++) {
        for (int j = 4 - 1; j < 5; j++) {
            score += evaluate_line(board[i][j], board[i+1][j-1], board[i+2][j-2], board[i+3][j-3], player);
        }
    }
}