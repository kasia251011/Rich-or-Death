#ifndef _PLAYER_H_
#define _PLAYER_H_

#include <stdlib.h>
#include <unistd.h>

#include "game.h"

int get_player_index(Player_t * Players);
void render_map(Player_t * Player, Window_t * Window, int round);

#endif