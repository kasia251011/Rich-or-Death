#ifndef _SERVER_H_
#define _SERVER_H_

#include "game.h"

//shm
void load_shm(Player_t ** Players, int ** Menager);
void close_shm(Player_t ** Players, int * Menager);

//board
void random_filed(Coordinates_t *);
int load_board();
void board_init();
void refresh_server(Window_t * Window, Player_t ** players);

//switch
void select_action_server(int action_id, Window_t * Window);
void select_action_player(Player_t *, Window_t * Window);


//server actions
void add_coin(char type);

//player actions
void move_left(Player_t * Player, Window_t * Window);
void move_right(Player_t * Player, Window_t * Window);
void move_up(Player_t * Player, Window_t * Window);
void move_down(Player_t * Player, Window_t * Window);

void players_init(Player_t **, int * round);
void delete_player(Player_t *, int index);
void send_map_to_player(Player_t *);

#endif