#ifndef _SERVER_H_
#define _SERVER_H_

#include <pthread.h>
#include "game.h"

extern Window_t __Window__;
extern Shm_game_t * __Shm_game__;
extern Beast_t __Beasts__[BEAST_MAX];
extern int __beast_counter__;

void random_filed(Coordinates_t *);
int load_board();
void board_init();
void refresh_server();

//switch
void select_action_server(Action_id_t action_id);
void select_action_player(Player_t *);


//server actions
void add_coin(char type);

//player actions
void player_move(Player_t *, int y, int x);
void players_init(Player_t * Players);
void delete_player(Player_t * Player, int index);
void send_map_to_player(Player_t * Player);

//beast actions
void beast_init();
void * beast_select_move(void *);
void beast_move(Beast_t *, int y, int x);
void push_beasts();
int chase(Beast_t *);

#endif