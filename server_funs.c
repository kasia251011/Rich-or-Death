#include "server_funs.h"

Blocks_t Blocks;
Beast_t __Beasts__[BEAST_MAX];
char __board_str__[BOARD_HEIGHT][BOARD_WIDTH];
int __dropped_treasure__[BOARD_HEIGHT][BOARD_WIDTH];
int __beast_counter__ = 0;


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

void board_init(Coordinates_t * camp_coords){
  load_board();
  
  //campasite
  random_filed(camp_coords);
  __board_str__[camp_coords->y][camp_coords->x] = 'A';

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
  mvwprintw(Window->stats, 1, 1, "Server's PID: %d", getpid());
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
    case ACTION_BEAST:

      if(__beast_counter__ == 4) break;
        random_filed(&(__Beasts__[__beast_counter__].coords));
        __Beasts__[__beast_counter__].block_before = '.';
        pthread_create(&(__Beasts__[__beast_counter__].thread), NULL, beast_move, __Beasts__ + __beast_counter__);

        __beast_counter__++;    

      break;
    default:
      break;
  }

}

void add_coin(char type){
  
  Coordinates_t coords;
  random_filed(&coords);
  __board_str__[coords.y][coords.x] = type;
}

void select_action_player(Player_t * Player, Window_t * Window){
  if(Player->pid == NO_PROCESS) return;
  if(Player->in_bushes){
    Player->in_bushes = 0;
    return;
  }

  switch (Player->action_id){
    case QUIT:
      break;
    case KEY_RIGHT:
      move_right(Player);
      break;
    case KEY_LEFT:
      move_left(Player);
      break;
    case KEY_UP:
      move_up(Player);
      break;
    case KEY_DOWN:
      move_down(Player);
      break;
    case NO_ACTION:
       __board_str__[Player->coords.y][Player->coords.x] = Player->icon;
      break;
    default:
      break;
  }
}

void move_right(Player_t * Player){
  int way = 1;

  switch (__board_str__[Player->coords.y][Player->coords.x + 1] ){
  case '|': return;
  case '#': Player->in_bushes = 1; break;
  case 'c': Player->coins_found ++; way = 0; break;
  case 't': Player->coins_found += 10; way = 0; break;
  case 'T': Player->coins_found += 50; way = 0; break;
  case 'D': Player->coins_found += __dropped_treasure__[Player->coords.y ][Player->coords.x + 1]; break;
  case '*':
    __board_str__[Player->coords.y][Player->coords.x + 1] = 'D';
    __dropped_treasure__[Player->coords.y][Player->coords.x + 1] = Player->coins_found;
    Player->coords.y = Player->coords_spawn.y;
    Player->coords.x = Player->coords_spawn.x;
    Player->coins_found = 0;
    return;
  case 'A': 
    Player->coins_brought += Player->coins_found;
    Player->coins_found = 0;
    break;
  case '1':
  case '2':
  case '3':
  case '4':
    break;
  default:
    break;
  }

  //path
  
  __board_str__[Player->coords.y][Player->coords.x] = Player->block_before;

  if(way){
    Player->block_before = __board_str__[Player->coords.y][Player->coords.x + 1];
  }
  

  Player->coords.x++;

  __board_str__[Player->coords.y][Player->coords.x] = Player->icon;
}

void move_left(Player_t * Player){
  int way = 1;

  switch (__board_str__[Player->coords.y][Player->coords.x - 1] ){
  case '|': return;
  case '#': Player->in_bushes = 1; break;
  case 'c': Player->coins_found ++; way = 0; break;
  case 't': Player->coins_found += 10; way = 0; break;
  case 'T': Player->coins_found += 50; way = 0; break;
  case 'D': Player->coins_found += __dropped_treasure__[Player->coords.y][Player->coords.x - 1]; break;
  case '*':
    __board_str__[Player->coords.y][Player->coords.x - 1] = 'D';
    __dropped_treasure__[Player->coords.y][Player->coords.x - 1] = Player->coins_found;
    Player->coords.y = Player->coords_spawn.y;
    Player->coords.x = Player->coords_spawn.x;
    Player->coins_found = 0;
    return;
  case 'A': 
    Player->coins_brought += Player->coins_found;
    Player->coins_found = 0;
    break;
  case '1':
  case '2':
  case '3':
  case '4':
    break;
  default:
    break;
  }

  //path
 
  __board_str__[Player->coords.y][Player->coords.x] = Player->block_before;
  if(way){  
    Player->block_before = __board_str__[Player->coords.y][Player->coords.x - 1];
  }
  

  Player->coords.x--;

  __board_str__[Player->coords.y][Player->coords.x] = Player->icon;
}

void move_up(Player_t * Player){
  int way = 1;

  switch (__board_str__[Player->coords.y - 1][Player->coords.x] ){
  case '|': return;
  case '#': Player->in_bushes = 1; break;
  case 'c': Player->coins_found ++; way = 0; break;
  case 't': Player->coins_found += 10; way = 0; break;
  case 'T': Player->coins_found += 50; way = 0; break;
  case 'D': Player->coins_found += __dropped_treasure__[Player->coords.y - 1][Player->coords.x]; break;
  case '*':
    __board_str__[Player->coords.y - 1][Player->coords.x] = 'D';
    __dropped_treasure__[Player->coords.y - 1][Player->coords.x] = Player->coins_found;
    Player->coords.y = Player->coords_spawn.y;
    Player->coords.x = Player->coords_spawn.x;
    Player->coins_found = 0;
    return;
  case 'A': 
    Player->coins_brought = Player->coins_found;
    Player->coins_found += 0;
    break;
  case '1':
  case '2':
  case '3':
  case '4':
    break;
  default:
    break;
  }

  //path
  
  __board_str__[Player->coords.y][Player->coords.x] = Player->block_before;
  if(way){  
    Player->block_before = __board_str__[Player->coords.y - 1][Player->coords.x];
  }
  

  Player->coords.y--;

  __board_str__[Player->coords.y][Player->coords.x] = Player->icon;
}

void move_down(Player_t * Player){
  int way = 1;

  switch (__board_str__[Player->coords.y + 1][Player->coords.x]){
  case '|': return;
  case '#': Player->in_bushes = 1; break;
  case 'c': Player->coins_found ++; way = 0; break;
  case 't': Player->coins_found += 10; way = 0; break;
  case 'T': Player->coins_found += 50; way = 0; break;
  case 'D': Player->coins_found += __dropped_treasure__[Player->coords.y + 1][Player->coords.x]; break;
  case '*':
    __board_str__[Player->coords.y + 1][Player->coords.x] = 'D';
    __dropped_treasure__[Player->coords.y + 1][Player->coords.x] = Player->coins_found;
    Player->coords.y = Player->coords_spawn.y;
    Player->coords.x = Player->coords_spawn.x;
    Player->coins_found = 0;
    return;
  case 'A': 
    Player->coins_brought = Player->coins_found;
    Player->coins_found += 0;
    break;
  case '1':
  case '2':
  case '3':
  case '4':

    break;
  default:
    break;
  }

  //path
  __board_str__[Player->coords.y][Player->coords.x] = Player->block_before;

  if(way){
    Player->block_before = __board_str__[Player->coords.y + 1][Player->coords.x];
  }
  

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
    players[i].in_bushes = 0;

    sem_init(&(players[i].sem_print_map), 1, 1);

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

void * beast_move(void * arg){
  Beast_t * Beast = (Beast_t *)arg;

  while(1){
    Direction_t dir = rand() % 4;
    sleep(1);
    switch (dir){
      case UP: beast_move_up(Beast); break;
      case DOWN: beast_move_down(Beast); break;
      case RIGHT: beast_move_right(Beast); break;
      case LEFT: beast_move_left(Beast); break;
      
      default:
        break;
    } 
  }
}


void beast_move_left(Beast_t * Beast){
  switch (__board_str__[Beast->coords.y][Beast->coords.x - 1] ){
  case '|':
  case '#': 
  case '*':
    return;
  case '1':
  case '2':
  case '3':
  case '4':
    //kill
    break;
  default:
    break;
  }

  //path
 
  __board_str__[Beast->coords.y][Beast->coords.x] = Beast->block_before;
  Beast->block_before = __board_str__[Beast->coords.y][Beast->coords.x - 1];
  

  Beast->coords.x--;

  __board_str__[Beast->coords.y][Beast->coords.x] = '*';
}

void beast_move_right(Beast_t * Beast){
  switch (__board_str__[Beast->coords.y][Beast->coords.x + 1] ){
  case '|':
  case '#': 
  case '*':
    return;
  case '1':
  case '2':
  case '3':
  case '4':
    //kill
    break;
  default:
    break;
  }

  //path
 
  __board_str__[Beast->coords.y][Beast->coords.x] = Beast->block_before;
  Beast->block_before = __board_str__[Beast->coords.y][Beast->coords.x + 1];
  

  Beast->coords.x++;

  __board_str__[Beast->coords.y][Beast->coords.x] = '*';
}

void beast_move_up(Beast_t * Beast){

  switch (__board_str__[Beast->coords.y - 1][Beast->coords.x] ){
  case '|':
  case '#': 
  case '*':
    return;
  case '1':
  case '2':
  case '3':
  case '4':
    //kill
    break;
  default:
    break;
  }

  //path
 
  __board_str__[Beast->coords.y][Beast->coords.x] = Beast->block_before;
  Beast->block_before = __board_str__[Beast->coords.y - 1][Beast->coords.x];
  

  Beast->coords.y--;

  __board_str__[Beast->coords.y][Beast->coords.x] = '*';
}

void beast_move_down(Beast_t * Beast){
switch (__board_str__[Beast->coords.y + 1][Beast->coords.x ] ){
  case '|':
  case '#': 
  case '*':
    return;
  case '1':
  case '2':
  case '3':
  case '4':
    //kill
    break;
  default:
    break;
  }

  //path
 
  __board_str__[Beast->coords.y][Beast->coords.x] = Beast->block_before;
  Beast->block_before = __board_str__[Beast->coords.y + 1][Beast->coords.x];
  

  Beast->coords.y++;

  __board_str__[Beast->coords.y][Beast->coords.x] = '*';
}