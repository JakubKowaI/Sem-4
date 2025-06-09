#pragma once
#include <stdlib.h>
#include <time.h>
#include "board.h"
#include "Logic.h"

int defensive_heuristic(const int board[5][5], int player);

int aggressive_heuristic(const int board[5][5], int player);

int balanced_heuristic(const int board[5][5], int player);