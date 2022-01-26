#ifndef _SERVER_H_
#define _SERVER_H_

#include <pthread.h>
#include "game.h"

void random_filed(Coordinates_t *);
int load_board();
void board_init(Coordinates_t * camp_coords);
void refresh_server(Window_t * Window, Player_t * players, int round);

//switch
void select_action_server(Action_id_t action_id, Window_t * Window);
void select_action_player(Player_t *, Window_t * Window);


//server actions
void add_coin(char type);

//player actions
void move_left(Player_t * Player);
void move_right(Player_t * Player);
void move_up(Player_t * Player);
void move_down(Player_t * Player);

void players_init(Player_t * Players);
void delete_player(Player_t * Player, int index);
void send_map_to_player(Player_t * Player);

//beast actions
void * beast_move(void *);
void beast_move_left(Beast_t *);
void beast_move_right(Beast_t *);
void beast_move_up(Beast_t *);
void beast_move_down(Beast_t *);


#endif