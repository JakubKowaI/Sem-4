#include "Heuristic.h"

static int evaluate_4_line_defensive(const int line[4],int player){
  int value=0;

  //Player = X
  //Opponent = O
  //Free space = _
  //Free/occupied = -

  if (is_win_pattern(line, player)) return INT_MAX;//XXXX
  if (is_win_pattern(line, switchPlayers(player))) return INT_MIN;//OOOO
  if (is_three_pattern(line, player)) return INT_MIN;//XXX- v -XXX
  if (is_three_pattern(line, switchPlayers(player))) return INT_MAX; //OOO- v -OOO
  if (is_pattern(line,player)) value+=250000; //XX_X v X_XX
  if (is_pattern(line,switchPlayers(player))) value-=100000; //OO_O v O_OO
  if (interrupted_pattern(line,player))value-=150000; //XXOX v XOXX
  if (interrupted_pattern(line,switchPlayers(player)))value+=350000; //OOXO v OXOO
  if (is_empty_pattern(line,player)) value+=75000; // X__X
  if (is_free_pattern(line,player))value+=65000;//_XX_
  if (is_one_side_blocked_pattern(line,player))value-=25000;//OXX_ v _XXO
  if (is_one_side_blocked_pattern(line,switchPlayers(player)))value+=125000;//XOO_ v _OOX
  if (is_checkered_pattern(line,player))value+=25000;//X_X_ v _X_X
  if (is_checkered_pattern(line,switchPlayers(player)))value-=15000;//O_O_ v _O_O
  if (is_two_on_the_side_pattern(line,player)) value+=7500;//XX__ v __XX 
  if (is_blind_two_on_the_side_pattern(line,player)) value-=12500;//XX_O v O_XX 
  return value;
}

int defensive_heuristic(const int board[5][5], int player) {
    int value=0;


  //Sprawdzanie poziomych linii po 4 znaki
  for (int i = 0; i < 5; i++) {
      for (int j = 0; j <= 1; j++) {
          int line[4];
          for (int k = 0; k < 4; k++) line[k] = board[i][j + k];
          value+=evaluate_4_line_defensive(line,player);
      }
  }

  //Sprawdzanie pionowych linii po 4 znaki
  for (int j = 0; j < 5; j++) {
      for (int i = 0; i <= 1; i++) {
          int line[4];
          for (int k = 0; k < 4; k++) line[k] = board[i + k][j];
          value+=evaluate_4_line_defensive(line,player);
      }
  }

  //Sprawdzanie ukośnych linii po 4 znaki (lewo-góra do prawo-dół)
  for (int i = 0; i <= 1; i++) {
      for (int j = 0; j <= 1; j++) {
          int line[4];
          for (int k = 0; k < 4; k++) line[k] = board[i + k][j + k];
          value+=evaluate_4_line_defensive(line,player);
      }
  }

  //Sprawdzanie ukośnych linii po 4 znaki (prawo-góra do lewo-dół)
  for (int i = 0; i <= 1; i++) {
      for (int j = 4; j >= 3; j--) {
          int line[4];
          for (int k = 0; k < 4; k++) line[k] = board[i + k][j - k];
          value+=evaluate_4_line_defensive(line,player);
      }
  }

  int threeLines[4][3]={
   { board[0][2], board[1][3], board[2][4]},
   { board[2][0], board[3][1], board[4][2] },
   { board[0][2], board[1][1], board[2][0] },
   { board[2][4], board[3][3], board[4][2] }};
    
  int opponent=switchPlayers(player);

  for(int i=0;i<4;i++){
    if(threeLines[i][0]==player&&threeLines[i][1]==player&&threeLines[i][2]==player)return INT_MIN;
    if(threeLines[i][0]==opponent&&threeLines[i][1]==opponent&&threeLines[i][2]==opponent)return INT_MAX;
  }

  for(int i=0;i<5;i++){
    for(int j=0;j<5;j++){
      if(board[i][j]==player){
        value += 25000 * (3 - max(abs(i - 2), abs(j - 2)));
      }
    }
  }
    //printf("Final value: %d\n%d\n",value,tempPlayer);

  return value; 
}

static int evaluate_4_line_aggresive(const int line[4],int player){
  int value=0;
  //value+=rand()%250;

  //Player = X
  //Opponent = O
  //Free space = _
  //Free/occupied = -

  if (is_win_pattern(line, player)) return INT_MAX;//XXXX
  if (is_win_pattern(line, switchPlayers(player))) return INT_MIN;//OOOO
  if (is_three_pattern(line, player)) return INT_MIN;//XXX- v -XXX
  if (is_three_pattern(line, switchPlayers(player))) return INT_MAX; //OOO- v -OOO
  if (is_pattern(line,player)) value+=1000000; //XX_X v X_XX
  if (is_pattern(line,switchPlayers(player))) value-=1250000; //OO_O v O_OO
  if (interrupted_pattern(line,player))value-=150000; //XXOX v XOXX
  if (interrupted_pattern(line,switchPlayers(player)))value+=2250000; //OOXO v OXOO
  if (is_empty_pattern(line,player)) value+=25000; // X__X
  if (is_free_pattern(line,player))value+=35000;//_XX_
  if (is_one_side_blocked_pattern(line,player))value-=35000;//OXX_ v _XXO
  if (is_one_side_blocked_pattern(line,switchPlayers(player)))value+=125000;//XOO_ v _OOX
  if (is_checkered_pattern(line,player))value+=50000;//X_X_ v _X_X
  if (is_checkered_pattern(line,switchPlayers(player)))value-=20000;//O_O_ v _O_O
  if (is_two_on_the_side_pattern(line,player)) value+=10000;//XX__ v __XX 
  if (is_blind_two_on_the_side_pattern(line,player)) value-=12500;//XX_O v O_XX 
  return value;
}

int aggressive_heuristic(const int board[5][5], int player) {
  int value=0;


  //Sprawdzanie poziomych linii po 4 znaki
  for (int i = 0; i < 5; i++) {
      for (int j = 0; j <= 1; j++) {
          int line[4];
          for (int k = 0; k < 4; k++) line[k] = board[i][j + k];
          value+=evaluate_4_line_aggresive(line,player);
      }
  }

  //Sprawdzanie pionowych linii po 4 znaki
  for (int j = 0; j < 5; j++) {
      for (int i = 0; i <= 1; i++) {
          int line[4];
          for (int k = 0; k < 4; k++) line[k] = board[i + k][j];
          value+=evaluate_4_line_aggresive(line,player);
      }
  }

  //Sprawdzanie ukośnych linii po 4 znaki (lewo-góra do prawo-dół)
  for (int i = 0; i <= 1; i++) {
      for (int j = 0; j <= 1; j++) {
          int line[4];
          for (int k = 0; k < 4; k++) line[k] = board[i + k][j + k];
          value+=evaluate_4_line_aggresive(line,player);
      }
  }

  //Sprawdzanie ukośnych linii po 4 znaki (prawo-góra do lewo-dół)
  for (int i = 0; i <= 1; i++) {
      for (int j = 4; j >= 3; j--) {
          int line[4];
          for (int k = 0; k < 4; k++) line[k] = board[i + k][j - k];
          value+=evaluate_4_line_aggresive(line,player);
      }
  }

  int threeLines[4][3]={
   { board[0][2], board[1][3], board[2][4]},
   { board[2][0], board[3][1], board[4][2] },
   { board[0][2], board[1][1], board[2][0] },
   { board[2][4], board[3][3], board[4][2] }};
    
  int opponent=switchPlayers(player);

  for(int i=0;i<4;i++){
    if(threeLines[i][0]==player&&threeLines[i][1]==player&&threeLines[i][2]==player)return INT_MIN;
    if(threeLines[i][0]==opponent&&threeLines[i][1]==opponent&&threeLines[i][2]==opponent)return INT_MAX;
  }


    for(int i=0;i<5;i++){
      for(int j=0;j<5;j++){
        if(board[i][j]==player){
          value += 500 * (3 - max(abs(i - 2), abs(j - 2)));
        }
      }
    }
    //printf("Final value: %d\n%d\n",value,tempPlayer);

    return value; 
}

int balanced_heuristic(const int board[5][5], int player) {
    int score = 0;

    //score += count_opportunities(board, me, 4) * 800;
    //score += count_opportunities(board, me, 3) * 200;
    //score -= count_threats(board, opponent(me), 4) * 700;
    //score -= count_threats(board, opponent(me), 3) * 400;

    return score;
}