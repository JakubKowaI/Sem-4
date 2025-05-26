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

// Sprawdza, czy w tablicy `line` o długości 4 występuje wzorzec XX-X lub X-XX
bool is_pattern(const int line[4], int player) {
    // Sprawdź XX-X (np. X X _ X)
    for (int k = 0; k < 4; k++) printf("%d",line[k]);
    printf("\n");
    bool pattern1 = (line[0] == player && line[1] == player && line[3] == player && line[2] == 0);
    // Sprawdź X-XX (np. X _ X X)
    bool pattern2 = (line[0] == player && line[2] == player && line[3] == player && line[1] == 0);
    return pattern1 || pattern2;
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

int aggressive_heuristic(int board[5][5],int tempPlayer) {
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            printf("%d",board[i][j]);
        }
        printf("\n");
    }
    int value=0;
    if(check_line(board,tempPlayer,4)){
      value+=5000;
      printf("Has 4 line value: %d\n",value);
    }else{
      if(check_line(board,tempPlayer,3)){
      value+=-5000;
            printf("Has 3 line value: %d\n",value);

      }
    }
    if(has_gap_line(board,tempPlayer)){
      value+=1000;      
      printf("Has gap line value: %d\n",value);
    }

    if(check_line(board,tempPlayer,2)){
      value+=250;      
      printf("Has 2 line value: %d\n",value);

    }
    for(int i=0;i<5;i++){
      for(int j=0;j<5;j++){
        if(board[i][j]==tempPlayer)value += 10 * (3 - max(abs(i - 2), abs(j - 2)));
      }
    }
    printf("Final value: %d\n%d\n",value,tempPlayer);

    return value; 
}

int main(){
    int board[5][5] = {
        {1, 0, 2, 0, 0},
        {0, 1, 0, 0, 0},
        {0, 0, 2, 0, 0},
        {1, 0, 2, 2, 0},
        {0, 1, 0, 0, 0}
    };
    printf("%d\n",aggressive_heuristic(board,2));
    return 0;
}