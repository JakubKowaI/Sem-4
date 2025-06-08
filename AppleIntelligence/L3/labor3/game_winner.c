/****************************
Maciej Gębala (CC BY-NC 4.0) & Jakub Kowal :)
Client ver. 0.2
2025-04-13
****************************/
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <limits.h>

#include "./board.h"
#include "./Heuristic.h"
#include "./Logic.h"

int player;

int (*h)(const int [5][5],int);
// int (*h_odp)(const int [5][5],int);
int myMax(int board[5][5],int player,int depth,int alpha,int beta);
int myMin(int board[5][5],int player,int depth,int alpha,int beta);

int generateStates(int board[5][5], int tempPlayer,int out[25]) {
    int counter = 0;
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            if (board[i][j] == 0) {
                out[counter++] = (i * 10 + j)+11;
            }
        }
    }
    return counter;
}

void apply_move(int board[5][5], int move, int player) {
    int row = (move/10)-1;
    int col = (move %10)-1;
    board[row][col] = player;
}

int myMax(int board[5][5],int player,int depth,int alpha,int beta){
  if(h(board,player)==INT_MAX)return INT_MAX;
  if(h(board,player)==INT_MIN)return INT_MIN;
  if(depth==0)return h(board,player);

  int moves[25];
  int count = generateStates(board, player,moves);
  int max=INT_MIN;

  for(int i=0;i<count;i++){
      int tempBoard[5][5];
      memcpy(tempBoard, board, sizeof(tempBoard));
      apply_move(tempBoard,moves[i],player);

      if(h(tempBoard,player)==INT_MAX)return INT_MAX;
  }

  for(int i=0;i<count;i++){
      int tempBoard[5][5];
      memcpy(tempBoard, board, sizeof(tempBoard));
      apply_move(tempBoard,moves[i],player);

      int temp=myMin(tempBoard,player,depth-1,alpha,beta);

      max = (temp > max) ? temp : max;
      alpha = (alpha >= temp) ? alpha : temp;
        if (beta <= alpha) break; 
    }
    return max;
}

int myMin(int board[5][5],int player,int depth,int alpha,int beta){
  if(h(board,player)==INT_MAX)return INT_MIN;
  if(h(board,player)==INT_MIN)return INT_MAX;
  if(depth==0)return h(board,player);

  int moves[25];
  int count = generateStates(board, switchPlayers(player),moves);
  int min=INT_MAX;

  for(int i=0;i<count;i++){
      int tempBoard[5][5];
      memcpy(tempBoard, board, sizeof(tempBoard));
      apply_move(tempBoard,moves[i],switchPlayers(player));

      int temp=myMax(tempBoard,player,depth-1,alpha,beta);

      min = (temp < min) ? temp : min;
      beta = (beta <= temp) ? beta : temp;
        if (beta <= alpha) break; 
    }
    return min;
}

int minmax(int board[5][5], int tempPlayer, int tempDepth){
  if(board[2][2]==0)return 33;
  int finalMove=-2;
  int alpha=INT_MIN;
  int beta=INT_MAX;

  int moves[25];
  int count = generateStates(board, tempPlayer,moves);

  if (count == 0)return -1;

  
    int max=INT_MIN;

    for(int i=0;i<count;i++){
      int tempBoard[5][5];
      memcpy(tempBoard, board, sizeof(tempBoard));
      apply_move(tempBoard,moves[i],tempPlayer);

      if(h(tempBoard,tempPlayer)==INT_MAX)return INT_MAX;
    }

    for(int i=0;i<count;i++){
      int tempBoard[5][5];
      memcpy(tempBoard, board, sizeof(tempBoard));
      apply_move(tempBoard,moves[i],tempPlayer);

      int temp=myMin(tempBoard,tempPlayer,tempDepth-1,alpha,beta);

      if(temp>max){
        max=temp;
        finalMove=moves[i];
      }
      alpha = (alpha >= temp) ? alpha : temp;
        if (beta <= alpha) break; 
    }
    printf("Final score: %d\n",max);
    return finalMove;
  
}

int main(int argc, char *argv[]) {
  srand(time(NULL));
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
  }else{
    h = defensive_heuristic;
  }

  //h =aggressive_heuristic;
  //h_odp = aggressive_heuristic;

  int depth = atoi(argv[5]);
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

      int thisMove=minmax(board,player,depth);

      printf("Final best choice %d :\n",thisMove);
      // for (int i = 0; i < 5; i++) {
      //     for (int j = 0; j < 5; j++) {
      //         printf("%d",thisMove.board[i][j]);
      //     }
      //     printf("\n");
      // }
      printf("%d\n",thisMove);
      snprintf(player_message, sizeof(player_message), "%d", thisMove);
      move=thisMove;

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
