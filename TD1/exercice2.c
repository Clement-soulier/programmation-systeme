#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>

void INIT_SCREEN(void){
    initscr();
    raw();
    keypad(stdscr, TRUE);
    noecho();
}

void DONE_SCREEN(void){
    endwin();
    exit(0);
}

#define PLAYERS 2
char CHIP[PLAYERS] = "XO#@";

#define SIZE 7
char board[SIZE][SIZE];

void init_board(void){
    for(int i = 0; i < SIZE; i++){
        for(int j = 0; j < SIZE; j++){
            board[i][j] = ' ';
        }
    }
}


void draw_board(void){
    move(0, 0);
    for(int line = SIZE - 1; line > -1; line--){
        for(int col = 0; col < SIZE; col++){
            printw("|%c", board[line][col]);
        }
        printw("|\n");
    }
    for(int i = 0; i < SIZE; i++){
        printw("+-");
    }
    printw("+\n");
    for(int i = 97; i - 97 < SIZE; i++){
        printw(" %c", i);
    }
  	printw("\n");
    refresh();
}

int get_col(void){
   do{
       int input = getch();
        if(input == KEY_BACKSPACE){
            return -1;
        }else
        if (input > 96 && input < 96 + SIZE + 1){
            return input - 97;
        }
    }while(1);
}

int define_next_player(int player){
    player++;
    if(player == PLAYERS){
        return 0;
    }
    return player;
}

int add_coin(int col, int player){
    int line = 0;
    while(line < SIZE){
        if(board[line][col] == ' '){
            board[line][col] = CHIP[player];
            return define_next_player(player);
        }
        line++;
    }
    return player;
}


int is_there_a_winner(void){
    //parcours par ligne
    int count = 0;
    int chip = board[0][0];
    for(int line = 0; line < SIZE; line++){
        for(int col = 0; col < SIZE; col++){
            if(board[line][col] == chip && chip != 32){
                count++;
                if(count == 4){
                    return chip;
                }
            }
            else{
                chip = board[line][col];
                count = 1;
            }
        }
    }
    //parcours par colonne
    count = 0;
    chip = board[0][0];
    for(int col = 0; col < SIZE; col++){
        for(int line = 0; line < SIZE; line++){
            if(board[line][col] == chip && chip != 32){
                count++;
                if(count == 4){
                    return chip;
                }
            }
            else{
                chip = board[line][col];
                count = 1;
            }
        }
    }
    //parcours par diagonale gauche
    count = 0;
    chip = board[0][3];
    int col;
    for(int start_col = 3; start_col < SIZE; start_col++){
        col = start_col;
        for(int line = 0; line < start_col + 1; line++){
            if(board[line][col] == chip && chip != 32){
                count++;
                if(count == 4){
                    return chip;
                }
            }
            else{
                chip = board[line][col];
                count = 1;
            }
            col--;
        }
    }
  	for(int start_line = 1; start_line < SIZE - 3; start_line++){
  		col = SIZE - 1;
  		for(int line = start_line; line < SIZE; line++){
  			if(board[line][col] == chip && chip != 32){
                count++;
                if(count == 4){
                    return chip;
                }
            }
            else{
                chip = board[line][col];
                count = 1;
            }
      		col--;
    	}
    }
  		
    //parcours diagonale droite
    count = 0;
    chip = board[0][0];
    for(int start_col = 0; start_col < SIZE - 3; start_col++){
      col = start_col;
      for(int line = 0; line < SIZE - start_col; line++){
        if(board[line][col] == chip && chip != 32){
          count++;
          if(count == 4){
            return chip;
          }
        }
        else{
          chip = board[line][col];
          count = 1;
        }
        col++;
      }
    }
  	for(int start_line = 1; start_line < SIZE - 3; start_line++){
      col = 0;
      for(int line = start_line; line < SIZE; line++){
        if(board[line][col] == chip && chip != 32){
          count++;
          if(count == 4){
            return chip;
          }
        }
        else{
          chip = board[line][col];
          count = 1;
        }
        col++;
      }
    }
    return 0;
}


int is_board_full(void){
    for(int line = 0; line < SIZE; line++){
        for(int col =0; col < SIZE; col++){
            if(board[line][col] == ' '){
                return 0;
            }
        }
    }
    return 1;
}

int game_over(void){
    int winner = is_there_a_winner();
    int board_full = is_board_full();
    if(!(winner) && !(board_full)){
        return 0;
    } else 
    if(!(winner) && board_full){
        return 1;
    }
    else{
        return winner;
    }
}

void play(void){
  	draw_board();
    int next_player = 0;
    int col = get_col();
  	int game_state;
  	if(!(col == -1)){
    	next_player = add_coin(col, next_player);
    	game_state = game_over();
    }
  	else{
      game_state = 2;
    }
    while(!(game_state)){
      	draw_board();
        col = get_col();
        next_player = add_coin(col, next_player);
        game_state = game_over();
    }
  	draw_board();
    if(game_state == 1){
        printw("La grille est pleine vous ne pouvez plus jouer");
      	refresh();
    }else 
    if(game_state == 2){
      printw("Vous avez quitté la partie");
    }
    else{
        printw("Le joueur %c à gagné", game_state);
      	refresh();
    }
}

int main(void){
    INIT_SCREEN();
    init_board();
    play();
    getch();
    DONE_SCREEN();
}
