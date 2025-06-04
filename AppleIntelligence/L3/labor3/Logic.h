#pragma once
#include <limits.h>

int max(int a, int b);

bool checkWinState(int tempBoard[5][5], int player);

int switchPlayers(int tempPlayer);

bool check_line(const int board[5][5], int player, int length);

bool is_pattern(const int line[4], int player);

bool is_three_pattern(const int line[4], int player);

bool is_win_pattern(const int line[4], int player);

bool interrupted_pattern(const int line[4], int player);

bool is_empty_pattern(const int line[4], int player);

bool is_free_pattern(const int line[4], int player);

bool is_one_side_blocked_pattern(const int line[4], int player);

bool is_checkered_pattern(const int line[4], int player);

bool is_two_on_the_side_pattern(const int line[4], int player);

bool is_blind_two_on_the_side_pattern(const int line[4], int player);

bool has_gap_line(const int board[5][5], int player);

int count_lines_of_length(const int board[5][5], int player, int length);

int count_gap_lines(const int board[5][5], int player);

int count_interruped_lines(const int board[5][5], int player);

bool one_gap_pattern(const int line[3], int player);

int count_lines_with_one_gap(const int board[5][5], int player);

int evaluate_line(int a, int b, int c, int d, int player);

int test(const int board[5][5],int player);