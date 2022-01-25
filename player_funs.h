#ifndef _PLAYER_H_
#define _PLAYER_H_

#include <errno.h>
#include "game.h"

//shm

int shm_load_menager(int ** Menager);
int get_player_key(int * Menager);
int shm_load_player(Player_t **, int * Menager);
void shm_close(Player_t *, int *);

int get_player_index(Player_t * Players);
void render_map(Player_t * Player, Window_t * Window);

#endif