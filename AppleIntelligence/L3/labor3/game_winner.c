/****************************
Maciej Gębala (CC BY-NC 4.0) & Jakub Kowal :)
Client ver. 0.2
2025-04-13
****************************/
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "./board.h"
#include "./Heuristic.h"
#include "./Logic.h"

typedef struct{
  int value;
  int board[5][5];
  int move;
  int aplha;
  int beta;
} state;

int depth=0;
int player;

int (*h_gracz)(const int [5][5],int);
int (*h_odp)(const int [5][5],int);

int generateStates(int board[5][5], int tempPlayer, state out[25]) {
    int counter = 0;
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            if (board[i][j] == 0) {
                state temp;
                board[i][j] = tempPlayer;

                memcpy(temp.board, board, sizeof(temp.board));
                temp.move = (i * 10 + j)+11;
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

//int licznik=0;

state minmax(int board[5][5], int tempPlayer, int tempDepth,int lastMove,int alpha, int beta){
  //licznik++;
  //printf("%d\r",licznik);
  state res;
  if(tempDepth == 0 || checkWinState(board, player) || checkWinState(board, switchPlayers(player))){
    if(tempPlayer==player){
      res.value = h_gracz(board,tempPlayer);
    }else{
      res.value = h_odp(board,tempPlayer);
    }
    
    res.move = lastMove;
    memcpy(res.board, board, sizeof(res.board));
    return res;
  }

  state *moves=malloc(25*sizeof(state));
  int count = generateStates(board, tempPlayer, moves);

  if (count == 0) {
    res.value = h_gracz(board, player);
    res.move = lastMove;
    memcpy(res.board, board, sizeof(res.board));
    return res;
  }

  if(tempPlayer==player){
     res.value=-999999;
    for(int i=0;i<count;i++){
      int temp=minmax(moves[i].board,switchPlayers(tempPlayer),tempDepth-1,moves[i].move,alpha,beta).value;
      if(temp>=res.value){
        res.value=temp;
        res.move=moves[i].move;
        memcpy(res.board, moves[i].board, sizeof(res.board));
      }
      alpha = (alpha > res.value) ? alpha : res.value;
        if (beta <= alpha) break; 
    }
    free(moves);
    // printf("najlepsze rozwiazanie o wartosci %d :\n",res.value);
    // for (int i = 0; i < 5; i++) {
    //     for (int j = 0; j < 5; j++) {
    //         printf("%d",res.board[i][j]);
    //     }
    //     printf("\n");
    // }
    //     printf("\n Last move %d\n",res.move);

    return res;
  }else{
  res.value=999999;
  for(int i=0;i<count;i++){
    int temp=minmax(moves[i].board,switchPlayers(tempPlayer),tempDepth-1,moves[i].move,alpha,beta).value;
    if(temp<=res.value){
      res.value=temp;
      res.move=moves[i].move;
      memcpy(res.board, moves[i].board, sizeof(res.board));
    }
    beta = (beta < res.value) ? beta : res.value;
      if (beta <= alpha) break;
  }
  free(moves);
    // printf("najlepsze rozwiazanie o wartosci %d :\n",res.value);
    // for (int i = 0; i < 5; i++) {
    //     for (int j = 0; j < 5; j++) {
    //         printf("%d",res.board[i][j]);
    //     }
    //     printf("\n");
    // }
    // printf("\n Last move %d\n",res.move);
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

  // if (player == 1){
  // h_gracz = aggressive_heuristic;
  // h_odp = defensive_heuristic;
  //  printf("Taking offensive approach\n");
  // }else{
  //  h_gracz = defensive_heuristic;
  //  h_odp= aggressive_heuristic;
  //  printf("Taking defensive approach\n");
  // }

  h_gracz = aggressive_heuristic;
  h_odp = aggressive_heuristic;

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
      
      printf("Current board before minmax():\n");
      printBoard();
      state thisMove=minmax(board,player,depth,-1,-999999,999999);
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
