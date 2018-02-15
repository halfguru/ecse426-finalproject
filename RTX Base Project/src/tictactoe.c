#include "tictactoe.h"

int state = 0;

void board_init(int board[9]){
	for(int i = 0; i < 9; i++){
		board[i] = 0;
	};
}

int moveblock(int board[9]){
	unsigned wins[8][3] = {{0,1,2},{3,4,5},{6,7,8},{0,3,6},{1,4,7},{2,5,8},{0,4,8},{2,4,6}};
	int i;
	for(i = 0; i < 8; ++i) {
		if(board[wins[i][0]] + board[wins[i][1]] + board[wins[i][2]] == 2){
			for(int j = 0; j < 3; j++){
				if(board[wins[i][j]] == 0)
					return wins[i][j];
			}
		}
	}
	return 10;
}

int movewin(int board[9]){
	unsigned wins[8][3] = {{0,1,2},{3,4,5},{6,7,8},{0,3,6},{1,4,7},{2,5,8},{0,4,8},{2,4,6}};
	int i;
	for(i = 0; i < 8; ++i) {
		if(board[wins[i][0]] + board[wins[i][1]] + board[wins[i][2]] == -2){
			for(int j = 0; j < 3; j++){
				if(board[wins[i][j]] == 0)
					return wins[i][j];
			}
		}
	}
	return 10;
}

int checkWin(int board[9]) {
    //determines if a player has won, returns 0 otherwise.
    unsigned wins[8][3] = {{0,1,2},{3,4,5},{6,7,8},{0,3,6},{1,4,7},{2,5,8},{0,4,8},{2,4,6}};
    int i;
    for(i = 0; i < 8; ++i) {
        if(board[wins[i][0]] != 0 &&
           board[wins[i][0]] == board[wins[i][1]] &&
           board[wins[i][0]] == board[wins[i][2]])
            return i;
    }
    return 9;
}

int AI(int board[9]){
	
	int move = movewin(board);
	if(move != 10){
		board[move] = -1;
		return move + 1;
	}
	move = moveblock(board);
	if(move != 10){
		board[move] = -1;
		return move + 1;
	}
	
	int next[9] = {0,8,2,6,4,1,3,5,7};
	for(int i = 0; i < 9; i++){
		if(board[next[i]] == 0){
			board[next[i]] = -1;
			return next[i] + 1;
		}
	}
	return 0;

}


