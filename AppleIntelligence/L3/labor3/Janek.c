/****************************
Maciej Gębala (CC BY-NC 4.0)
Random Boot ver. 0.2
2025-04-13
****************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <arpa/inet.h>
#include <stdbool.h>

#include "./board.h"
#include "./Heuristic.h"

const int SCORE_WIN = 1000000;
const int SCORE_LOSE = -1000000;
const int MAKE_THREE = -500000;
const int OPPONENT_THREE = 500000;

const int SCORE_X_XX_PATTERN = 100;
const int SCORE_O_OO_PATTERN = -100;
const int SCORE_X__X_PATTERN = 10;
const int SCORE_O__O_PATTERN = -10;
const int SCORE_XX_PATTERN = 10;
const int SCORE_OO_PATTERN = -10;
const int SCORE_X_ENDS_PATTERN = -5;
const int SCORE_O_ENDS_PATTERN = 5;

int evaluateBoard(int player_evaluating_for);
int minimax(int depth, bool isMaximizing, int player_id, int alpha, int beta);
int findBestMove(int player, int depth);

int evaluateBoard(int player_evaluating_for) {
    int score = 0;
    int my_id = player_evaluating_for;
    int op_id = 3 - my_id;

    for (int i = 0; i < 28; i++) {
        int p[4];
        int my_symbols_in_line = 0;
        int op_symbols_in_line = 0;
        int empty_symbols_in_line = 0;

        for(int k=0; k<4; ++k) {
            p[k] = board[win[i][k][0]][win[i][k][1]];
            if (p[k] == my_id) my_symbols_in_line++;
            else if (p[k] == op_id) op_symbols_in_line++;
            else empty_symbols_in_line++;
        }

        if (op_symbols_in_line == 0) {
            if (my_symbols_in_line == 3 && empty_symbols_in_line == 1) {
                if ((p[0] == my_id && p[1] == 0 && p[2] == my_id && p[3] == my_id) ||
                    (p[0] == my_id && p[1] == my_id && p[2] == 0 && p[3] == my_id)) {
                     score += SCORE_X_XX_PATTERN;
                }
            }
            if (my_symbols_in_line == 2 && empty_symbols_in_line == 2) {
                if (p[0] == my_id && p[1] == 0 && p[2] == 0 && p[3] == my_id) {
                     score += SCORE_X__X_PATTERN;
                }
            }
        }
        
        if (my_symbols_in_line == 0) {
            if (op_symbols_in_line == 3 && empty_symbols_in_line == 1) {
                if ((p[0] == op_id && p[1] == 0 && p[2] == op_id && p[3] == op_id) ||
                    (p[0] == op_id && p[1] == op_id && p[2] == 0 && p[3] == op_id)) {
                    score += SCORE_O_OO_PATTERN;
                }
            }
            if (op_symbols_in_line == 2 && empty_symbols_in_line == 2) {
                if (p[0] == op_id && p[1] == 0 && p[2] == 0 && p[3] == op_id) {
                    score += SCORE_O__O_PATTERN;
                }
            }
        }
    }

    // int r_idx, c_idx;
    // for (r_idx = 0; r_idx < 5; r_idx++) {
    //     for (c_idx = 0; c_idx < 4; c_idx++) {
    //         if (board[r_idx][c_idx] == my_id && board[r_idx][c_idx+1] == my_id) score += SCORE_XX_PATTERN;
    //         if (board[r_idx][c_idx] == op_id && board[r_idx][c_idx+1] == op_id) score += SCORE_OO_PATTERN;
    //     }
    // }
    // for (c_idx = 0; c_idx < 5; c_idx++) {
    //     for (r_idx = 0; r_idx < 4; r_idx++) {
    //         if (board[r_idx][c_idx] == my_id && board[r_idx+1][c_idx] == my_id) score += SCORE_XX_PATTERN;
    //         if (board[r_idx][c_idx] == op_id && board[r_idx+1][c_idx] == op_id) score += SCORE_OO_PATTERN;
    //     }
    // }
    // for (r_idx = 0; r_idx < 4; r_idx++) {
    //     for (c_idx = 0; c_idx < 4; c_idx++) {
    //         if (board[r_idx][c_idx] == my_id && board[r_idx+1][c_idx+1] == my_id) score += SCORE_XX_PATTERN;
    //         if (board[r_idx][c_idx] == op_id && board[r_idx+1][c_idx+1] == op_id) score += SCORE_OO_PATTERN;
    //     }
    // }
    // for (r_idx = 0; r_idx < 4; r_idx++) {
    //     for (c_idx = 1; c_idx < 5; c_idx++) {
    //         if (board[r_idx][c_idx] == my_id && board[r_idx+1][c_idx-1] == my_id) score += SCORE_XX_PATTERN;
    //         if (board[r_idx][c_idx] == op_id && board[r_idx+1][c_idx-1] == op_id) score += SCORE_OO_PATTERN;
    //     }
    // }

    for (int r = 0; r < 5; ++r) {
        if (board[r][0] == my_id && board[r][4] == my_id) {
            if (board[r][1] != my_id && board[r][2] != my_id && board[r][3] != my_id) {
                score += SCORE_X_ENDS_PATTERN;
            }
        }
        if (board[r][0] == op_id && board[r][4] == op_id) {
            if (board[r][1] != op_id && board[r][2] != op_id && board[r][3] != op_id) {
                score += SCORE_O_ENDS_PATTERN;
            }
        }
    }

    for (int c = 0; c < 5; ++c) {
        if (board[0][c] == my_id && board[4][c] == my_id) {
            if (board[1][c] != my_id && board[2][c] != my_id && board[3][c] != my_id) {
                score += SCORE_X_ENDS_PATTERN;
            }
        }
        if (board[0][c] == op_id && board[4][c] == op_id) {
            if (board[1][c] != op_id && board[2][c] != op_id && board[3][c] != op_id) {
                score += SCORE_O_ENDS_PATTERN;
            }
        }
    }

    if (board[0][0] == my_id && board[4][4] == my_id) {
        if (board[1][1] != my_id && board[2][2] != my_id && board[3][3] != my_id) {
            score += SCORE_X_ENDS_PATTERN;
        }
    }
    if (board[0][0] == op_id && board[4][4] == op_id) {
        if (board[1][1] != op_id && board[2][2] != op_id && board[3][3] != op_id) {
            score += SCORE_O_ENDS_PATTERN;
        }
    }

    if (board[0][4] == my_id && board[4][0] == my_id) {
        if (board[1][3] != my_id && board[2][2] != my_id && board[3][1] != my_id) {
            score += SCORE_X_ENDS_PATTERN;
        }
    }
    if (board[0][4] == op_id && board[4][0] == op_id) {
        if (board[1][3] != op_id && board[2][2] != op_id && board[3][1] != op_id) {
            score += SCORE_O_ENDS_PATTERN;
        }
    }

    return score;
}

int minimax(int depth, bool isMaximizing, int player_id, int alpha, int beta) {
    int opponent_id = 3 - player_id;

    if (winCheck(player_id)) return SCORE_WIN;
    if (winCheck(opponent_id)) return SCORE_LOSE;
    if (loseCheck(player_id)) return MAKE_THREE;
    if (loseCheck(opponent_id)) return OPPONENT_THREE;

    bool isFull = true;
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            if (board[i][j] == 0) {
                isFull = false;
                break;
            }
        }
        if (!isFull) break;
    }
    if (isFull) return 0;

    if (depth == 0) {
        return aggressive_heuristic(player_id);
    }

    if (isMaximizing) {
        int maxEval = -200000;
        for (int r = 0; r < 5; r++) {
            for (int c = 0; c < 5; c++) {
                if (board[r][c] == 0) {
                    board[r][c] = player_id;
                    int eval = minimax(depth - 1, false, player_id, alpha, beta);
                    board[r][c] = 0;
                    if (eval > maxEval) maxEval = eval;
                    if (alpha < eval) alpha = eval;
                    if (beta <= alpha) goto end_max_loops;
                }
            }
        }
        end_max_loops:;
        return maxEval;
    } else {
        int minEval = 200000;
        for (int r = 0; r < 5; r++) {
            for (int c = 0; c < 5; c++) {
                if (board[r][c] == 0) {
                    board[r][c] = opponent_id;
                    int eval = minimax(depth - 1, true, player_id, alpha, beta);
                    board[r][c] = 0;
                    if (eval < minEval) minEval = eval;
                    if (beta > eval) beta = eval;
                    if (beta <= alpha) goto end_min_loops;
                }
            }
        }
        end_min_loops:;
        return minEval;
    }
}

int findBestMove(int player, int depth) {
    int bestScore = -20000000;
    int bestMove = -1;
    int alpha = -20000000;
    int beta = 20000000;

    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            if (board[i][j] == 0) {
                board[i][j] = player;
                int moveScore = minimax(depth - 1, false, player, alpha, beta);
                board[i][j] = 0;

                if (moveScore > bestScore) {
                    bestScore = moveScore;
                    bestMove = (i + 1) * 10 + (j + 1);
                }
                
                if (moveScore > alpha) {
                    alpha = moveScore;
                }
                
                if (beta <= alpha) {
                    goto end_loops;
                }
            }
        }
    }
    end_loops:;

    if (bestMove == -1) {
        for (int i = 0; i < 5; i++) {
            for (int j = 0; j < 5; j++) {
                if (board[i][j] == 0) {
                    return (i + 1) * 10 + (j + 1);
                }
            }
        }
    }
    
    return bestMove;
}

int main(int argc, char *argv[]) {
  int server_socket;
  struct sockaddr_in server_addr;
  char server_message[16], player_message[16];

  bool end_game;
  int player, msg, move, depth;

  if (argc != 6) {
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
  depth = atoi(argv[5]);

  while (!end_game) {
    memset(server_message, '\0', sizeof(server_message));
    if (recv(server_socket, server_message, sizeof(server_message), 0) < 0) {
      printf("Error while receiving server's message\n");
      return -1;
    }
    sscanf(server_message, "%d", &msg);
    move = msg%100;
    msg = msg/100;
    if ( move != 0 ) {
      setMove(move, 3-player);
    }
    if ((msg == 0) || (msg == 6)) {
      move = findBestMove(player, depth);
      setMove(move, player);
      memset(player_message, '\0', sizeof(player_message));
      snprintf(player_message, sizeof(player_message), "%d", move);
      if (send(server_socket, player_message, strlen(player_message), 0) < 0) {
        printf("Unable to send message\n");
        return -1;
      }
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