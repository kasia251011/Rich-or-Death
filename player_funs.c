#include "player_funs.h"

int get_player_index(Player_t * Players){
  for(int i = 0; i < PLAYERS_MAX; i++){
    if(Players[i].pid == NO_PROCESS) return i;
  }

  return -1;
}

void render_map(Player_t * Player, Window_t * Window, int round, pid_t server_pid){
  Blocks_t Blocks;
  blocks_init(&Blocks);
  chtype block;

  for (int i = 0; i < MAP_HEIGHT; ++i){
    for (int j = 0; j < MAP_WIDTH; ++j){
      switch (Player->view[i][j]){
        case '|': block = Blocks.Wall; break;
        case '.': block = Blocks.Path; break;
        case '#': block = Blocks.Bushes; break;
        case 'A': block = Blocks.Campasite; break;
        case 'c': block = Blocks.OneCoin; break;
        case 't': block = Blocks.Treasue; break;
        case 'T': block = Blocks.Large_Treasure; break;
        case 'D': block = Blocks.Treasue; break;
        case '*': block = Blocks.Beast; break;
        case '1': block = Blocks.Players[0]; break;
        case '2': block = Blocks.Players[1]; break;
        case '3': block = Blocks.Players[2]; break;
        case '4': block = Blocks.Players[3]; break;
        default:
          block = Blocks.Bushes;
          break;
      }

      mvwaddch(Window->board, Player->coords.y + i - 2, Player->coords.x + j - 2, block);

      if(Player->view[i][j] == 'A'){
        mvwprintw(Window->stats, 2, 1, " Campasite X/Y: %02d/%02d", j, i);
      }
    }
  }

  wrefresh(Window->board);
  mvwprintw(Window->stats, 1, 1, "Server's PID: %d", server_pid);
  mvwprintw(Window->stats, 3, 2, "Round number: %d", round);
  mvwprintw(Window->stats, 5, 1, "Player:");
  mvwprintw(Window->stats, 6, 2, "Number:        %d", Player->number);
  mvwprintw(Window->stats, 7, 2, "Curr X/Y       %02d/%02d", Player->coords.x, Player->coords.y);
  mvwprintw(Window->stats, 8, 2, "Deaths         %d", Player->deaths);

  mvwprintw(Window->stats, 11, 2,"Coins found    %d", Player->coins_found);
  mvwprintw(Window->stats, 12, 2,"Coins brought  %d", Player->coins_brought);

  wrefresh(Window->stats);

}

void clear_map(Window_t * Window){
  for(int i = 0; i < BOARD_HEIGHT; i++){
    for(int j = 0; j < BOARD_WIDTH; j++){
      mvwaddch(Window->board, i, j, ' ');
    }
  }
}