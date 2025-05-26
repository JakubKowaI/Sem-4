/****************************
Maciej Gębala (CC BY-NC 4.0) & Jakub Kowal :)
Client ver. 0.2
2025-04-13
****************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "./board.h"

typedef struct{
  int value;
  int board[5][5];
  int move;
} state;

int depth=0;
int player;

int (*h)(int [5][5],int);

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

int generateStates(int board[5][5], int tempPlayer, state out[25]) {
    int counter = 0;
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            if (board[i][j] == 0) {
                state temp;
                board[i][j] = tempPlayer;

                memcpy(temp.board, board, sizeof(temp.board));
                temp.move = i * 10 + j;
                temp.value = 0;

                out[counter++] = temp;
                // printf("%d:\n",i);
                // for (int i = 0; i < 5; i++) {
                //     for (int j = 0; j < 5; j++) {
                //         printf("%d",temp.board[i][j]);
                //     }
                //     printf("\n");
                // }
                board[i][j] = 0;
            }
        }
    }
    return counter;
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
    bool pattern1 = (line[0] == player && line[1] == player && line[3] == player && line[2] == '0');
    // Sprawdź X-XX (np. X _ X X)
    bool pattern2 = (line[0] == player && line[2] == player && line[3] == player && line[1] == '0');
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

int switchPlayers(int tempPlayer){
  return 3-tempPlayer;
}

int aggressive_heuristic(int board[5][5],int tempPlayer) {
    int value=0;
    if(check_line(board,tempPlayer,4)){
      value+=5000;
    }else{
      if(check_line(board,tempPlayer,3)){
      value+=-5000;
      }
    }
    if(has_gap_line(board,tempPlayer))value+=1000;
    if(check_line(board,tempPlayer,2)){
      value+=250;
    }
    for(int i=0;i<5;i++){
      for(int j=0;j<5;j++){
        if(board[i][j]==tempPlayer)value+=(5-(abs(i-3)+abs(j-3)))*10;
      }
    }

    return value; 
}

int defensive_heuristic(int board[5][5],int tempPlayer) {
    int value=0;
    if(check_line(board,tempPlayer,4)){
      value+=5000;
    }else{
      if(check_line(board,tempPlayer,3)){
      value+=-5000;
      }
    }
    if(has_gap_line(board,tempPlayer))value+=1000;
    if(check_line(board,tempPlayer,2)){
      value+=250;
    }
    for(int i=0;i<5;i++){
      for(int j=0;j<5;j++){
        if(board[i][j]==tempPlayer)value+=(5-(abs(i-3)+abs(j-3)))*10;
      }
    }

    return value; 
}
int licznik=0;
state think(int board[5][5], int tempPlayer, int tempDepth,int lastMove){
  licznik++;
  //printf("%d\r",licznik);
  state res;
  if(tempDepth==0||checkWinState(board,tempPlayer)){
    res.value = h(board,tempPlayer);
    res.move = lastMove;
    memcpy(res.board, board, sizeof(res.board));
    return res;
  }

  state *moves=malloc(25*sizeof(state));
  int count = generateStates(board, tempPlayer, moves);

  if(tempPlayer==player){
     res.value=-9999;
    for(int i=0;i<count;i++){
      int temp=think(moves[i].board,switchPlayers(tempPlayer),tempDepth-1,moves[i].move).value;
      if(temp>=res.value){
        res.value=temp;
        res.move=moves[i].move;
        memcpy(res.board, moves[i].board, sizeof(res.board));
      }
    }
    free(moves);
    printf("najlepsze rozwiazanie o wartosci %d :\n",res.value);
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            printf("%d",res.board[i][j]);
        }
        printf("\n");
    }
        printf("\n Last move %d\n",res.move);

    return res;
  }else{
    res.value=9999;
    for(int i=0;i<count;i++){
      int temp=think(moves[i].board,switchPlayers(tempPlayer),tempDepth-1,moves[i].move).value;
      if(temp<=res.value){
        res.value=temp;
        res.move=moves[i].move;
        memcpy(res.board, moves[i].board, sizeof(res.board));
      }
    }
    free(moves);
    printf("najlepsze rozwiazanie o wartosci %d :\n",res.value);
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            printf("%d",res.board[i][j]);
        }
        printf("\n");
    }
    printf("\n Last move %d\n",res.move);
    return res;
  }
}

int main(int argc, char *argv[]) {
  int server_socket;
  struct sockaddr_in server_addr;
  char server_message[16], player_message[16];

  bool end_game;
  int  msg, move;

  if ( argc != 6 ) {
    printf("Wrong number of arguments\n");
    return -1;
  }

  // Create socket
  server_socket = socket(AF_INET, SOCK_STREAM, 0);
  if ( server_socket < 0 ) {
    printf("Unable to create socket\n");
    return -1;
  }
  printf("Socket created successfully\n");

  // Set port and IP the same as server-side
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(atoi(argv[2]));
  server_addr.sin_addr.s_addr = inet_addr(argv[1]);

  // Send connection request to server
  if ( connect(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0 ) {
    printf("Unable to connect\n");
    return -1;
  }
  printf("Connected with server successfully\n");

  // Receive the server message
  memset(server_message, '\0', sizeof(server_message));
  if ( recv(server_socket, server_message, sizeof(server_message), 0) < 0 ) {
    printf("Error while receiving server's message\n");
    return -1;
  }
  printf("Server message: %s\n", server_message);

  memset(player_message, '\0', sizeof(player_message));
  snprintf(player_message, sizeof(player_message), "%s %s", argv[3], argv[4]);
  // Send the message to server
  if ( send(server_socket, player_message, strlen(player_message), 0) < 0 ) {
    printf("Unable to send message\n");
    return -1;
  }

  setBoard();
  end_game = false;
  sscanf(argv[3], "%d", &player);

  if (player == 1){
    h = aggressive_heuristic;
    printf("Taking offensive approach\n");
  }else{
    h = defensive_heuristic;
    printf("Taking defensive approach\n");
}

  depth = atoi(argv[5]);
  if(depth>10||depth<1)return -1;
  while ( !end_game ) {
    memset(server_message, '\0', sizeof(server_message));
    if ( recv(server_socket, server_message, sizeof(server_message), 0) < 0 ) {
      printf("Error while receiving server's message\n");
      return -1;
    }
    printf("Server message: %s\n", server_message);
    sscanf(server_message, "%d", &msg);
    move = msg%100;
    msg = msg/100;
    if ( move != 0 ) {
      setMove(move, 3-player);
      printBoard();
    }
    if ( (msg == 0) || (msg == 6) ) {
      printf("Your move: ");
      // if ( fgets(player_message, sizeof(player_message), stdin) == NULL ) {
      //   printf("Error while reading move\n");
      //   return -1;
      // }
      
      printf("Current board before think():\n");
      printBoard();
      state thisMove=think(board,player,depth,0);
      printf("Final best choice %d :\n",thisMove.value);
      for (int i = 0; i < 5; i++) {
          for (int j = 0; j < 5; j++) {
              printf("%d",thisMove.board[i][j]);
          }
          printf("\n");
      }
      printf("%d\n",thisMove.move);
      snprintf(player_message, sizeof(player_message), "%d", thisMove.move);
      move=thisMove.move;

      sscanf(player_message, "%d", &move);
      setMove(move, player);
      printBoard();
      memset(player_message, '\0', sizeof(player_message));
      snprintf(player_message, sizeof(player_message), "%d", move);
      if ( send(server_socket, player_message, strlen(player_message), 0) < 0 ) {
        printf("Unable to send message\n");
        return -1;
      }
      printf("Player message: %s\n", player_message);
     } else {
       end_game = true;
       switch ( msg ) {
         case 1 : printf("You won.\n"); break;
         case 2 : printf("You lost.\n"); break;
         case 3 : printf("Draw.\n"); break;
         case 4 : printf("You won. Opponent error.\n"); break;
         case 5 : printf("You lost. Your error.\n"); break;
      }
    }
  }

  // Close socket
  close(server_socket);

  return 0;
}
