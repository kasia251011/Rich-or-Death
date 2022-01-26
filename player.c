
#include <sys/ipc.h>
#include <sys/shm.h>
#include <pthread.h>
#include <errno.h>
#include <signal.h>
#include <string.h>

#include "player_funs.h"

Shm_game_t * __Shm_game__;
Window_t __Window__;
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

  __Shm_game__ = (Shm_game_t *)shmat(shm_id, NULL, 0);
  __index__ = get_player_index(__Shm_game__->Players);

  if(__index__ < 0){
    printf("Couldn't join to tha game, too many players");
    return 0;
  }

  __Shm_game__->Players[__index__].pid = getpid();

  screen_init();
  window_init(&__Window__);

  pthread_t get_key_thread, show_map_thread;

  pthread_create(&get_key_thread, NULL, get_key, NULL);
  pthread_create(&show_map_thread, NULL, show_map, NULL);

  pthread_join(get_key_thread, NULL);

  __Shm_game__->Players[__index__].pid = -3;

  screen_destroy();

  shmdt(__Shm_game__);
  
  return 0;
}

void * get_key(void * arg){
  char action;

  while(1){
    __Shm_game__->Players[__index__].action_id = mvwgetch(__Window__.input, 1, 1);   

    if(__Shm_game__->Players[__index__].action_id == QUIT) break;
  }

  return NULL;
}

void * show_map(void * arg){
  int i = 0;

  while(1){
    //nie działa

    if(kill(__Shm_game__->server_pid, 0) != 0){
      mvwprintw(__Window__.terminal, 1, 1, "Server nie istnieje");
      wrefresh(__Window__.terminal);
      sleep(1);
      exit(1);
    }

    //

    sem_wait(&(__Shm_game__->Players[__index__].sem_print_map));

    clear_map(&__Window__);
    render_map(__Shm_game__->Players + __index__);
  }
}