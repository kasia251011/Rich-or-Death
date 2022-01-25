#include "server_funs.h"

Blocks_t Blocks;
char __board_str__[BOARD_HEIGHT][BOARD_WIDTH];

void random_filed(Coordinates_t * coords){
  srand(time(NULL));

  do{
    coords->x = rand() % BOARD_WIDTH;
    coords->y = rand() % BOARD_HEIGHT;
  }while(__board_str__[coords->y][coords->x ] != '.');

}

int load_board(){
    FILE *f;
    if ((f = fopen(BOARD_TXT, "r")) == NULL ) return 2;

    int row = 0;

    while(!feof(f) && row < BOARD_HEIGHT){
        fscanf(f, "%s", __board_str__[row++]);
    }

    fclose(f);
    return 0;
}

void board_init(){
  load_board();
  
  //campasite
  Coordinates_t campasite_coords;
  random_filed(&campasite_coords);
  __board_str__[campasite_coords.y][campasite_coords.x] = 'A';

}

void refresh_server(Window_t * Window, Player_t * Players, int round){

  blocks_init(&Blocks);
  chtype block;

  for (int i = 0; i < BOARD_HEIGHT; ++i){
    for (int j = 0; j < BOARD_WIDTH; ++j){
      switch (__board_str__[i][j]){
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
          break;
      }

      mvwaddch(Window->board, i, j, block);

      if(__board_str__[i][j] == 'A'){
        mvwprintw(Window->stats, 2, 1, " Campasite X/Y: %02d/%02d", j, i);
      }
    }
  }

  wrefresh(Window->board);

  //stats
  mvwprintw(Window->stats, 3, 2, "Round number: %d", round);
  mvwprintw(Window->stats, 5, 1, "Parameter:  Player1  Player2  Player3  Player4");

  for(int i = 0, x = 0; i < PLAYERS_MAX; i++, x+=9){
    if(Players[i].pid != NO_PROCESS){
      mvwprintw(Window->stats, 6, 13 + x, "%d", Players[i].pid);
    }else{
      mvwprintw(Window->stats, 6, 13+ x, "-      ");
    }

    if(Players[i].pid != NO_PROCESS){
      mvwprintw(Window->stats, 7, 13+ x, "%02d/%02d", Players[i].coords.x,Players[i].coords.y);
    }else{
      mvwprintw(Window->stats, 7, 13+ x, "--/--  ");
    }

    if(Players[i].pid != NO_PROCESS){
      mvwprintw(Window->stats, 8, 13+ x, "%d", Players[i].deaths);
    }else{
      mvwprintw(Window->stats, 8, 13+ x, "-  ");
    }

    if(Players[i].pid != NO_PROCESS){
      mvwprintw(Window->stats, 11, 13+ x, "%d", Players[i].coins_found);
    }

    if(Players[i].pid != NO_PROCESS){
      mvwprintw(Window->stats, 12, 13+ x, "%d", Players[i].coins_brought);
    }

  }

  mvwprintw(Window->stats, 6, 1, " PID       ");
  mvwprintw(Window->stats, 7, 1, " CURR X/Y  ");
  mvwprintw(Window->stats, 8, 1, " Deaths    ");

  mvwprintw(Window->stats, 10, 1, " Coins");
  mvwprintw(Window->stats, 11, 1, "   carried ");
  mvwprintw(Window->stats, 12, 1, "   brought ");

  wrefresh(Window->stats);

}

void select_action_server(Action_id_t action_id, Window_t * Window){
  blocks_init(&Blocks);

  switch (action_id){
    case QUIT:
      mvwprintw(Window->terminal, 1, 1, "Press any key to quit...");
      return;
    case ACTION_ONE_COIN:
      mvwprintw(Window->terminal, 1, 1, "Added One Coin");
      add_coin(ACTION_ONE_COIN);
      break;
    case ACTION_TREASURE:
      mvwprintw(Window->terminal, 1, 1, "Added Treasure");
      add_coin(ACTION_TREASURE);
      break;
    case ACTION_LARGE_TREASURE:
      mvwprintw(Window->terminal, 1, 1, "Added Large Treasure");
      add_coin(ACTION_LARGE_TREASURE);
      break;
    default:
      break;
  }

}

void add_coin(char type){
  
  Coordinates_t coords;
  random_filed(&coords);
  __board_str__[coords.x][coords.y] = type;
}

void select_action_player(Player_t * Player, Window_t * Window){
  if(Player->pid == NO_PROCESS) return;

  switch (Player->action_id){
    case QUIT:
      break;
    case KEY_RIGHT:
      move_right(Player, Window);
      break;
    case KEY_LEFT:
      move_left(Player, Window);
      break;
    case KEY_UP:
      move_up(Player, Window);
      break;
    case KEY_DOWN:
      move_down(Player, Window);
      break;
    case NO_ACTION:
       __board_str__[Player->coords.y][Player->coords.x] = Player->icon;
      break;
    default:
      break;
  }
}

void move_right(Player_t * Player, Window_t * Window){
  if(__board_str__[Player->coords.y][Player->coords.x + 1] != '.') return;

  //path
  __board_str__[Player->coords.y][Player->coords.x] = Player->block_before;
  Player->block_before = __board_str__[Player->coords.y][Player->coords.x + 1];

  Player->coords.x++;

  __board_str__[Player->coords.y][Player->coords.x] = Player->icon;
}

void move_left(Player_t * Player, Window_t * Window){
  if(__board_str__[Player->coords.y][Player->coords.x - 1] != '.') return;

  //path
  __board_str__[Player->coords.y][Player->coords.x] = Player->block_before;
  Player->block_before = __board_str__[Player->coords.y][Player->coords.x - 1];

  Player->coords.x--;

  __board_str__[Player->coords.y][Player->coords.x] = Player->icon;
}

void move_up(Player_t * Player, Window_t * Window){
  if(__board_str__[Player->coords.y - 1][Player->coords.x] != '.') return;

  //path
  __board_str__[Player->coords.y][Player->coords.x] = Player->block_before;
  Player->block_before = __board_str__[Player->coords.y - 1][Player->coords.x];

  Player->coords.y--;

  __board_str__[Player->coords.y][Player->coords.x] = Player->icon;
}

void move_down(Player_t * Player, Window_t * Window){
  if(__board_str__[Player->coords.y + 1][Player->coords.x] != '.') return;

  //path
  __board_str__[Player->coords.y][Player->coords.x] = Player->block_before;
  Player->block_before = __board_str__[Player->coords.y + 1][Player->coords.x];

  Player->coords.y++;

  __board_str__[Player->coords.y][Player->coords.x] = Player->icon;
}

void players_init(Player_t * players){

  for(int i = 0; i < PLAYERS_MAX; i++){
    players[i].action_id = NO_ACTION;
    players[i].pid = NO_PROCESS;
    players[i].icon = i + 1 + '0';
    players[i].block_before = '.';
    
    random_filed(&(players[i].coords_spawn));

    players[i].coords.x = players[i].coords_spawn.x; 
    players[i].coords.y = players[i].coords_spawn.y;

    players[i].coins_brought = 0;
    players[i].coins_found = 0;
    players[i].number = i+1;

  }
}

void delete_player(Player_t * Player, int index){
  Player->action_id = NO_ACTION;
  Player->pid = NO_PROCESS;
  Player->icon = index + 1 + '0';
  Player->block_before = '.';

  __board_str__[Player->coords.y][Player->coords.x] = '.';

  random_filed(&(Player->coords_spawn));

  Player->coords.x = Player->coords_spawn.x; 
  Player->coords.y = Player->coords_spawn.y;

  Player->coins_brought = 0;
  Player->coins_found = 0;
  Player->number = index + 1;
}

void send_map_to_player(Player_t * Player){
  for(int i = Player->coords.y - 2, k = 0; k < MAP_HEIGHT; i++, k++){
    for(int j = Player->coords.x - 2, d = 0; d < MAP_WIDTH; j++, d++){
      Player->view[k][d] = __board_str__[i][j];
    }
  }
}