#pragma once

int max(int a, int b);

bool checkWinState(int tempBoard[5][5], int player);

int switchPlayers(int tempPlayer);

bool check_line(const int board[5][5], int player, int length);

bool has_gap_line(const int board[5][5], int player);

int count_lines_of_length(const int board[5][5], int player, int length);

int count_gap_lines(const int board[5][5], int player);

int count_interruped_lines(const int board[5][5], int player);