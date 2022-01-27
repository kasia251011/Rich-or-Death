#include "player_funs.h"

int get_player_index(){
  for(int i = 0; i < PLAYERS_MAX; i++){
    if(__Shm_game__->Players[i].pid == NO_PROCESS) return i;
  }

  return -1;
}

void render_map(Player_t * Player){
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
        case '*': block = Blocks.Beast; break;
        case '1': block = Blocks.Players[0]; break;
        case '2': block = Blocks.Players[1]; break;
        case '3': block = Blocks.Players[2]; break;
        case '4': block = Blocks.Players[3]; break;
        default:
          block = Blocks.Bushes;
          break;
      }

      if(Player->dropped[i][j] > 0){
        block = Blocks.Dropped_Treasure;
      }

      mvwaddch(__Window__.board, Player->coords.y + i - 2, Player->coords.x + j - 2, block);

      if(Player->view[i][j] == 'A'){
        mvwprintw(__Window__.stats, 2, 1, " Campasite X/Y: %02d/%02d", __Shm_game__->camp_coords.x, __Shm_game__->camp_coords.y);
      }
    }
  }

  wrefresh(__Window__.board);
  mvwprintw(__Window__.stats, 1, 1, "Server's PID: %d", __Shm_game__->server_pid);
  mvwprintw(__Window__.stats, 3, 2, "Round number: %d", __Shm_game__->round);
  mvwprintw(__Window__.stats, 5, 1, "Player:");
  mvwprintw(__Window__.stats, 6, 2, "Number:        %d", Player->number);
  mvwprintw(__Window__.stats, 7, 2, "Curr X/Y       %02d/%02d", Player->coords.x, Player->coords.y);
  mvwprintw(__Window__.stats, 8, 2, "Deaths         %d", Player->deaths);

  mvwprintw(__Window__.stats, 11, 2,"Coins found    %d", Player->coins_found);
  mvwprintw(__Window__.stats, 12, 2,"Coins brought  %d", Player->coins_brought);

  wrefresh(__Window__.stats);

}

void clear_map(Window_t * Window){
  for(int i = 0; i < BOARD_HEIGHT; i++){
    for(int j = 0; j < BOARD_WIDTH; j++){
      mvwaddch(__Window__.board, i, j, ' ');
    }
  }
}