#include "server_funs.h"


Blocks_t __Blocks__;
int __beast_counter__ = 0;

char __board_str__[BOARD_HEIGHT][BOARD_WIDTH];
int __dropped_treasure__[BOARD_HEIGHT][BOARD_WIDTH];


void random_filed(Coordinates_t * coords){
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
  for (int i = 0; i < BOARD_HEIGHT; ++i){
    for (int j = 0; j < BOARD_WIDTH; ++j){
      __dropped_treasure__[i][j] = 0;
    }
  }

  random_filed(&(__Shm_game__->camp_coords));
  __board_str__[__Shm_game__->camp_coords.y][__Shm_game__->camp_coords.x] = 'A';
}

void refresh_server(){
  
  chtype block;

  for (int i = 0; i < BOARD_HEIGHT; ++i){
    for (int j = 0; j < BOARD_WIDTH; ++j){
      switch (__board_str__[i][j]){
        case '|': block = __Blocks__.Wall; break;
        case '.': block = __Blocks__.Path; break;
        case '#': block = __Blocks__.Bushes; break;
        case 'A': block = __Blocks__.Campasite; break;
        case 'c': block = __Blocks__.OneCoin; break;
        case 't': block = __Blocks__.Treasue; break;
        case 'T': block = __Blocks__.Large_Treasure; break;
        case '*': block = __Blocks__.Beast; break;
        case '1': block = __Blocks__.Players[0]; break;
        case '2': block = __Blocks__.Players[1]; break;
        case '3': block = __Blocks__.Players[2]; break;
        case '4': block = __Blocks__.Players[3]; break;
        default:
          break;
      }

      if(__dropped_treasure__[i][j] > 0 && __board_str__[i][j] != '*'){
        block = __Blocks__.Dropped_Treasure;
      }

      mvwaddch(__Window__.board, i, j, block);

      if(__board_str__[i][j] == 'A'){
        mvwprintw(__Window__.stats, 2, 1, " Campasite X/Y: %02d/%02d", j, i);
      }

      
    }
  }

  wrefresh(__Window__.board);

  //stats
  mvwprintw(__Window__.stats, 1, 1, "Server's PID: %d", getpid());
  mvwprintw(__Window__.stats, 3, 2, "Round number: %d", __Shm_game__->round);
  mvwprintw(__Window__.stats, 5, 1, "Parameter:  Player1  Player2  Player3  Player4");

  for(int i = 0, x = 0; i < PLAYERS_MAX; i++, x+=9){
    if(__Shm_game__->Players[i].pid != NO_PROCESS){
      mvwprintw(__Window__.stats, 6, 13 + x, "%d", __Shm_game__->Players[i].pid);
    }else{
      mvwprintw(__Window__.stats, 6, 13+ x, "-      ");
    }

    if(__Shm_game__->Players[i].pid != NO_PROCESS){
      mvwprintw(__Window__.stats, 7, 13+ x, "%02d/%02d", __Shm_game__->Players[i].coords.x,__Shm_game__->Players[i].coords.y);
    }else{
      mvwprintw(__Window__.stats, 7, 13+ x, "--/--  ");
    }

    if(__Shm_game__->Players[i].pid != NO_PROCESS){
      mvwprintw(__Window__.stats, 8, 13+ x, "%d", __Shm_game__->Players[i].deaths);
    }else{
      mvwprintw(__Window__.stats, 8, 13+ x, "-  ");
    }

    if(__Shm_game__->Players[i].pid != NO_PROCESS){
      mvwprintw(__Window__.stats, 11, 13+ x, "%d", __Shm_game__->Players[i].coins_found);
    }

    if(__Shm_game__->Players[i].pid != NO_PROCESS){
      mvwprintw(__Window__.stats, 12, 13+ x, "%d", __Shm_game__->Players[i].coins_brought);
    }

  }

  mvwprintw(__Window__.stats, 6, 1, " PID       ");
  mvwprintw(__Window__.stats, 7, 1, " CURR X/Y  ");
  mvwprintw(__Window__.stats, 8, 1, " Deaths    ");

  mvwprintw(__Window__.stats, 10, 1, " Coins");
  mvwprintw(__Window__.stats, 11, 1, "   carried ");
  mvwprintw(__Window__.stats, 12, 1, "   brought ");

  wrefresh(__Window__.stats);

}

void select_action_server(Action_id_t action_id){
  blocks_init(&__Blocks__);

  switch (action_id){
    case QUIT:
      mvwprintw(__Window__.terminal, 1, 1, "Press any key to quit...");
      return;
    case ACTION_ONE_COIN:
      mvwprintw(__Window__.terminal, 1, 1, "Added One Coin");
      add_coin(ACTION_ONE_COIN);
      break;
    case ACTION_TREASURE:
      mvwprintw(__Window__.terminal, 1, 1, "Added Treasure");
      add_coin(ACTION_TREASURE);
      break;
    case ACTION_LARGE_TREASURE:
      mvwprintw(__Window__.terminal, 1, 1, "Added Large Treasure");
      add_coin(ACTION_LARGE_TREASURE);
      break;
    case ACTION_BEAST:
      beast_init();

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

void select_action_player(Player_t * Player){
  if(Player->pid == NO_PROCESS) return;
  if(Player->in_bushes){
    Player->in_bushes = 0;
    return;
  }

  switch (Player->action_id){
    case QUIT:
      break;
    case KEY_RIGHT: player_move(Player, 0, 1); break;
    case KEY_LEFT: player_move(Player, 0, -1); break;
    case KEY_UP: player_move(Player, -1, 0); break;
    case KEY_DOWN: player_move(Player, 1, 0); break;
    case NO_ACTION:
       __board_str__[Player->coords.y][Player->coords.x] = Player->icon;
      break;
    default:
      break;
  }
}

void player_move(Player_t * Player, int y, int x){
  int way = 1;
  int second_player_number;

  switch (__board_str__[Player->coords.y + y][Player->coords.x + x] ){
  case '|': return;
  case '#': Player->in_bushes = 1; break;
  case 'c': Player->coins_found ++; way = 0; break;
  case 't': Player->coins_found += 10; way = 0; break;
  case 'T': Player->coins_found += 50; way = 0; break;
  case '*':
    __dropped_treasure__[Player->coords.y + y][Player->coords.x + x] = Player->coins_found;
    __board_str__[Player->coords.y][Player->coords.x] = Player->block_before;
    


    Player->coords.y = Player->coords_spawn.y;
    Player->coords.x = Player->coords_spawn.x;
    
    x = 0;
    y = 0;

    Player->coins_found = 0;
    Player->deaths++;
    return;
  case 'A': 
    Player->coins_brought += Player->coins_found;
    Player->coins_found = 0;
    break;
  case '1':
  case '2':
  case '3':
  case '4':
    
    second_player_number =  __board_str__[Player->coords.y + y][Player->coords.x + x] - '0';

    Player_t * second_player;

    for(int i = 0; i < PLAYERS_MAX; i++){
      if(second_player_number == __Shm_game__->Players[i].number){
        second_player = &__Shm_game__->Players[i];
      }
    }
    
    __dropped_treasure__[Player->coords.y + y][Player->coords.x + x] = Player->coins_found + second_player->coins_found;
    __board_str__[Player->coords.y + y][Player->coords.x + x] = second_player->block_before;

    __board_str__[Player->coords.y][Player->coords.x] = Player->block_before;
    Player->coins_found = 0;
    Player->deaths ++;
    Player->coords.y = Player->coords_spawn.y;
    Player->coords.x = Player->coords_spawn.x;
   

    x = 0;
    y = 0;

    second_player->coins_found = 0;
    second_player->deaths ++;
    second_player->coords.y = second_player->coords_spawn.y;
    second_player->coords.x = second_player->coords_spawn.x;
    

    break;
  default:
    break;
  }

  if(__dropped_treasure__[Player->coords.y + y][Player->coords.x + x] > 0){
    Player->coins_found += __dropped_treasure__[Player->coords.y + y][Player->coords.x + x]; 
    __dropped_treasure__[Player->coords.y + y][Player->coords.x + x] = 0;
  }

  //path
  
  __board_str__[Player->coords.y][Player->coords.x] = Player->block_before;

  if(way){
    Player->block_before = __board_str__[Player->coords.y + y][Player->coords.x + x];
  }

  Player->coords.x += x;
  Player->coords.y += y;

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
      Player->dropped[k][d] = __dropped_treasure__[i][j];
    }
  }
}

void beast_init(){
  if(__beast_counter__ == 4) return;
  
  random_filed(&(__Beasts__[__beast_counter__].coords));
  __Beasts__[__beast_counter__].block_before = '.';
  sem_init(&(__Beasts__[__beast_counter__].semaphore), 0, 1);
  pthread_create(&(__Beasts__[__beast_counter__].thread), NULL, beast_select_move, __Beasts__ + __beast_counter__);

  __beast_counter__++;    
}

void * beast_select_move(void * arg){
  Beast_t * Beast = (Beast_t *)arg;
  Direction_t dir;

  mvwprintw(__Window__.terminal, 1, 1, "%d %d %d %d", KEY_LEFT, KEY_RIGHT, KEY_DOWN, KEY_UP);
  wrefresh(__Window__.terminal);

  while(1){
    dir = chase(Beast);
    if(dir == -1){
      dir = rand() % 4;
    }
    
    sem_wait(&(Beast->semaphore));

    switch (dir){
      case UP: beast_move(Beast, -1, 0); break;
      case DOWN: beast_move(Beast, 1, 0); break;
      case RIGHT: beast_move(Beast, 0, 1); break;
      case LEFT: beast_move(Beast, 0, -1); break;
      
      default:
        break;
    } 
  }
}

void beast_move(Beast_t * Beast, int y, int x){
  int player_number;

  switch (__board_str__[Beast->coords.y + y][Beast->coords.x + x] ){
  case '|':
  case '#': 
  case '*':
  case 'A':
    return;
  case '1':
  case '2':
  case '3':
  case '4':
    player_number =  __board_str__[Beast->coords.y + y][Beast->coords.x + x] - '0';

    Player_t * player;

    for(int i = 0; i < PLAYERS_MAX; i++){
      if(player_number == __Shm_game__->Players[i].number){
        player = &__Shm_game__->Players[i];
      }
    }
    
    __dropped_treasure__[Beast->coords.y + y][Beast->coords.x + x] = player->coins_found;
    __board_str__[Beast->coords.y + y][Beast->coords.x + x] = player->block_before;

   // __board_str__[player->coords.y][Player->coords.x] = Player->block_before;
    player->coins_found = 0;
    player->deaths ++;
    player->coords.y = player->coords_spawn.y;
    player->coords.x = player->coords_spawn.x;
    
    break;
  default:
    break;
  }

  //path
 
  __board_str__[Beast->coords.y][Beast->coords.x] = Beast->block_before;
  Beast->block_before = __board_str__[Beast->coords.y + y][Beast->coords.x + x];
  

  Beast->coords.x += x;
  Beast->coords.y += y;

  __board_str__[Beast->coords.y][Beast->coords.x] = '*';
}

void push_beasts(){
  for(int  i = 0; i < __beast_counter__; i++){
    sem_post(&(__Beasts__[i].semaphore));
  }
}

int chase(Beast_t * Beast){
  int i = 0;

  while(__board_str__[Beast->coords.y + i][Beast->coords.x] != '|' && __board_str__[Beast->coords.y + i][Beast->coords.x] != '#' && __board_str__[Beast->coords.y + i][Beast->coords.x] != 'A' ){
    if(__board_str__[Beast->coords.y + i][Beast->coords.x] == '1' || __board_str__[Beast->coords.y + i][Beast->coords.x] == '2' || __board_str__[Beast->coords.y + i][Beast->coords.x] == '3' ||  __board_str__[Beast->coords.y + i][Beast->coords.x] == '4' ){
      return DOWN;
    }
    i++;
  }
  
  i = 0;
  while(__board_str__[Beast->coords.y - i][Beast->coords.x] != '|' && __board_str__[Beast->coords.y - i][Beast->coords.x] != '#' && __board_str__[Beast->coords.y - i][Beast->coords.x] != 'A' ){
    if(__board_str__[Beast->coords.y - i][Beast->coords.x] == '1' || __board_str__[Beast->coords.y - i][Beast->coords.x] == '2' || __board_str__[Beast->coords.y - i][Beast->coords.x] == '3' ||  __board_str__[Beast->coords.y - i][Beast->coords.x] == '4' ){
      return UP;
    }
    i++;
  }

  i = 0;
  while(__board_str__[Beast->coords.y ][Beast->coords.x + i] != '|' && __board_str__[Beast->coords.y][Beast->coords.x + i] != '#' && __board_str__[Beast->coords.y][Beast->coords.x + i] != 'A' ){
    if(__board_str__[Beast->coords.y ][Beast->coords.x + i] == '1' || __board_str__[Beast->coords.y][Beast->coords.x + i] == '2' || __board_str__[Beast->coords.y][Beast->coords.x + i] == '3' ||  __board_str__[Beast->coords.y][Beast->coords.x + i] == '4' ){
      return RIGHT;
    }
    i++;
  }

  i = 0;
  while(__board_str__[Beast->coords.y ][Beast->coords.x - i] != '|' && __board_str__[Beast->coords.y][Beast->coords.x - i] != '#' && __board_str__[Beast->coords.y][Beast->coords.x - i] != 'A' ){
    if(__board_str__[Beast->coords.y ][Beast->coords.x - i] == '1' || __board_str__[Beast->coords.y][Beast->coords.x - i] == '2' || __board_str__[Beast->coords.y][Beast->coords.x - i] == '3' ||  __board_str__[Beast->coords.y][Beast->coords.x - i] == '4' ){
      return LEFT;
    }
    i++;
  }

  return -1;
}