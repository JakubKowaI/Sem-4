/****************************
Maciej Gębala (CC BY-NC 4.0)
Plansza ver. 0.2
2025-04-13
****************************/
#pragma once
#include <stdio.h>

extern int board[5][5];

extern const int win[28][4][2];

extern const int lose[48][3][2];

void setBoard();

void printBoard();

bool setMove(int move, int player);

bool winCheck(int player);

bool loseCheck(int player);
