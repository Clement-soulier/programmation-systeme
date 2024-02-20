#include <ncurses.h>
#include <stdlib.h>
#include <stdio.h>

int define_players(int argc, char *argv[]){
  if(argc > 3){
        int player = (int) strtol(argv[3],  NULL, 10);
        if(player < 4){
            return 4;
        } else 
        if(player > 8){
            return 8;
        } else {
            return player;
        }
    }
    char *player_from_env = getenv("P4PLAYERS");
    if(player_from_env){
        return (int) strtol(player_from_env, NULL, 10);
    } else {
        if(setenv("P4PLAYERS", "2", 1)){
            exit(-1);
        }
    }
    return (int) strtol(getenv("P4PLAYERS"), NULL, 10);
}

int define_width(int argc, char *argv[]){
    if(argc > 1){
        int width = (int) strtol(argv[1],  NULL, 10);
        if(width < 4){
            return 4;
        } else 
        if(width > 26){
            return 26;
        } else {
            return width;
        }
    }
    char *width_from_env = getenv("P4WIDTH");
    if(width_from_env){
        return (int) strtol(width_from_env, NULL, 10);
    } else {
        if(setenv("P4WIDTH", "16", 1)){
            exit(-1);
        }
    }
    return (int) strtol(getenv("P4WIDTH"), NULL, 10);
}

int define_height(int argc, char *argv[]){
    if(argc > 2){
        int height = (int) strtol(argv[2],  NULL, 10);
        if(height < 4){
            return 4;
        } else 
        if(height > 16){
            return 16;
        } else {
            return height;
        }
    }
    char *height_from_env = getenv("P4HEIGHT");
    if(height_from_env){
        return (int) strtol(height_from_env, NULL, 10);
    } else {
        if(setenv("P4HEIGHT", "16", 1)){
            exit(-1);
        }
    }
    return (int) strtol(getenv("P4HEIGHT"), NULL, 10);
}

int PLAYERS, LINE, COL;
char CHIP[8] = "XO@ABCDE";
char board[26][26];

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

void draw_board(void){
    move(0, 0);
    for(int line = LINE - 1; line > -1; line--){
        for(int col = 0; col < COL; col++){
            printw("|%c", board[line][col]);
        }
        printw("|\n");
    }
    for(int i = 0; i < COL; i++){
        printw("+-");
    }
    printw("+\n");
    for(int i = 97; i - 97 < COL; i++){
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
        if (input > 96 && input < 96 + COL + 1){
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
    while(line < LINE){
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
    for(int line = 0; line < LINE; line++){
        for(int col = 0; col < COL; col++){
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
    for(int col = 0; col < COL; col++){
        for(int line = 0; line < LINE; line++){
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
    for(int start_col = 3; start_col < COL; start_col++){
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
  	for(int start_line = 1; start_line < LINE - 3; start_line++){
  		col = COL - 1;
  		for(int line = start_line; line < LINE; line++){
  			if(board[line][col] == chip && chip != 32){
                count++;
                if(count == 4){
                    return chip;
                }
            }else{
                chip = board[line][col];
                count = 1;
            }
      		col--;
    	}
    }
  		
    //parcours diagonale droite
    count = 0;
    chip = board[0][0];
    for(int start_col = 0; start_col < COL - 3; start_col++){
      col = start_col;
      for(int line = 0; line < LINE - start_col; line++){
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
  	for(int start_line = 1; start_line < LINE - 3; start_line++){
      col = 0;
      for(int line = start_line; line < LINE; line++){
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
    for(int line = 0; line < LINE; line++){
        for(int col =0; col < COL; col++){
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

void init_board(void){
    for(int i = 0; i < LINE; i++){
        for(int j = 0; j < COL; j++){
            board[i][j] = ' ';
        }
    }
}

int main(int argc, char *argv[]){
  	PLAYERS = define_players(argc, argv);
	LINE = define_height(argc, argv);
	COL = define_width(argc, argv);
    INIT_SCREEN();
    init_board();
    play();
    getch();
    DONE_SCREEN();
}
