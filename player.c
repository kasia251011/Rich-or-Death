#include <unistd.h> //sleep
#include <sys/ipc.h>
#include <sys/shm.h>
#include <pthread.h>
#include <errno.h>

#include "player_funs.h"

Window_t player_window;

Shm_game_t * Shm_game = NULL;
int __index__ = 0;

void * get_key(void * arg);
void * show_map(void * arg);

int main(){

  //GET PLAYER FROM SHM
  key_t KEY_SHM = 123;
  int shm_id = shmget(KEY_SHM, sizeof(Shm_game_t), 0666);

  if(errno == ENOENT){
    printf("First server must working\n");
    return 0;
  }

  Shm_game = (Shm_game_t *)shmat(shm_id, NULL, 0);
  
  __index__ = get_player_index(Shm_game->Players);

  if(__index__ < 0){
    printf("Couldn't join to tha game, too many players");
    return 0;
  }

  Shm_game->Players[__index__].pid = getpid();

  screen_init();
  window_init(&player_window);

  keypad(player_window.input, true);

  pthread_t get_key_thread, show_map_thread;

  pthread_create(&get_key_thread, NULL, get_key, NULL);
  pthread_create(&show_map_thread, NULL, show_map, NULL);

  pthread_join(get_key_thread, NULL);

  Shm_game->Players[__index__].pid = -3;

  screen_destroy();

  shmdt(Shm_game);
  
  return 0;
}

void * get_key(void * arg){
  char action;

  while(1){
    Shm_game->Players[__index__].action_id = mvwgetch(player_window.input, 1, 1);   

    if(Shm_game->Players[__index__].action_id == QUIT) break;
  }

  return NULL;
}

void * show_map(void * arg){
  while(1){
    sem_wait(&(Shm_game->Players[__index__].sem_print_map));
    clear_map(&player_window);
    render_map(Shm_game->Players + __index__, &player_window, Shm_game->round);
  }
}