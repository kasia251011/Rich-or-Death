#ifndef _SCREEN_H_
#define _SCREEN_H_

#include <ncurses.h>
#include <stdlib.h> //NULL
#include <time.h>
#include <unistd.h>
#include <semaphore.h>

#define WINDOW_HEIGHT 35
#define WINDOW_WIDTH 96
#define BOARD_HEIGHT 25
#define BOARD_WIDTH 51
#define STATS_HEIGHT 25
#define STATS_WIDTH 60
#define MAP_WIDTH 5
#define MAP_HEIGHT 5
#define BOARD_TXT "board.txt"

#define PLAYERS_MAX 4
#define BEAST_MAX 4
#define NO_PROCESS -2

void screen_init();
void screen_destroy();

typedef struct {
  WINDOW * board;
  WINDOW * stats;
  WINDOW * terminal;
  WINDOW * input;
} Window_t;

void window_init(Window_t *);

typedef struct {
  chtype Wall;
  chtype Path;
  chtype Bushes;
  chtype OneCoin;
  chtype Treasue;
  chtype Large_Treasure;
  chtype Dropped_Treasure;
  chtype Beast;
  chtype Campasite;
  chtype Players[PLAYERS_MAX];

} Blocks_t;

void blocks_init(Blocks_t *);

typedef struct {
  int x;
  int y;
} Coordinates_t;



typedef enum {ACTION_ONE_COIN = 'c', QUIT = 'q', NO_ACTION, ACTION_TREASURE = 't', ACTION_LARGE_TREASURE = 'T', ACTION_BEAST = 'b'} Action_id_t ;
typedef enum {LEFT, RIGHT, UP, DOWN} Direction_t;

typedef struct {
  int id;
  Coordinates_t coords;
} Action_t;

typedef struct {
  pid_t pid;
  char icon;
  char block_before;
  Coordinates_t coords_spawn;
  Coordinates_t coords;
  int action_id;
  char view[5][5];
  char dropped[5][5];
  int deaths;
  int number;
  int coins_found;
  int coins_brought;
  sem_t sem_print_map;
  int in_bushes;

} Player_t;

typedef struct {
  pthread_t thread;
  sem_t semaphore;
  Coordinates_t coords;
  char block_before;
} Beast_t;


typedef struct{
  Player_t Players[PLAYERS_MAX];
  int round;
  pid_t server_pid;
  Coordinates_t camp_coords;
} Shm_game_t;




#endif