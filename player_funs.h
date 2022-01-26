#ifndef _PLAYER_H_
#define _PLAYER_H_

#include "game.h"

extern Shm_game_t * __Shm_game__;
extern Window_t __Window__;

int get_player_index();
void render_map(Player_t * Player);
void clear_map();

#endif