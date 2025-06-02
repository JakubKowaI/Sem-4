#include "Heuristic.h"
#include "board.h"

int defensive_heuristic(const int board[5][5], int player) {
    int value=0;
    if(check_line(board,player,4)){
      value+=5000;
    }else{
      if(check_line(board,player,3)){
      value+=-5000;
      }
    }
    if(has_gap_line(board,player))value+=1000;
    if(check_line(board,player,2)){
      value+=250;
    }
    for(int i=0;i<5;i++){
      for(int j=0;j<5;j++){
        if(board[i][j]==player)value+=(5-(abs(i-4)+abs(j-4)))*10;
      }
    }
    return value; 
}

int aggressive_heuristic(const int board[5][5], int player) {
    // for (int i = 0; i < 5; i++) {
    //     for (int j = 0; j < 5; j++) {
    //         printf("%d",board[i][j]);
    //     }
    //     printf("\n");
    //}
    int value=0;
    value+=5000*count_lines_of_length(board,player,4);
    value-=5000*count_lines_of_length(board,switchPlayers(player),4);
    value-=1250*count_interruped_lines(board,switchPlayers(player),4);
    value+=2500*count_gap_lines(board,player);
    value+=-7000*count_lines_of_length(board,player,3);
    value+=250*count_lines_of_length(board,player,2);
    
    for(int i=0;i<5;i++){
      for(int j=0;j<5;j++){
        if(board[i][j]==player){
          value += 10 * (3 - max(abs(i - 2), abs(j - 2)));
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

int analyze_style(const int moves[],int opponent) {
    int blocks = 0, attacks = 0;

    for (int i = 0; i < 5; i++) {
        // if (moves[i].blocks_us) blocks++;
        // if (moves[i].creates_threat) attacks++;
    }

    // if (blocks > attacks * 1.5) return DEFENSIVE;
    // if (attacks > blocks * 1.5) return AGGRESSIVE;
    // return BALANCED;
    return 0;
}