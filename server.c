#include <unistd.h> //sleep

#include <pthread.h>
#include <signal.h>
#include <errno.h>
#include <string.h>
#include "server_funs.h"

Player_t * __Players__[PLAYERS_MAX];
int * __menager__;

void * exe_server_action(void * arg);
void * exe_players_action(void * arg);

Window_t server_window;
int __server_ation_id__ = NO_ACTION;
int __round__ = 0;

int main(){

  load_shm(__Players__, &__menager__);

  screen_init();
  window_init(&server_window);
  board_init();
  keypad(server_window.input, true);

  players_init(__Players__, &__round__);

  

  pthread_t exe_server_action_thread, exe_players_action_thread;

  pthread_create(&exe_server_action_thread, NULL, exe_server_action, NULL);
  pthread_create(&exe_players_action_thread, NULL, exe_players_action, NULL);

  pthread_join(exe_server_action_thread, NULL);
  pthread_join(exe_players_action_thread, NULL);

  screen_destroy();

  close_shm(__Players__, __menager__);
  
  return 0;
}



void * exe_server_action(void * arg){

  while(1){
    __server_ation_id__ = mvwgetch(server_window.input, 1, 1);
    select_action_server(__server_ation_id__, &server_window);
    refresh_server(&server_window, __Players__);
    if(__server_ation_id__ == QUIT) return NULL;
  }

  return NULL;
}


void * exe_players_action(void * arg){

  while (1){

    for(int i = 0; i < PLAYERS_MAX; i++){
      if(kill(__Players__[i]->pid, 0) != 0){

        //if process was already killed controled or no controled
        if(__Players__[i]->pid != NO_PROCESS){
          delete_player(__Players__[i], i);
          __menager__[i] = EMPTY;
        }
      }

      select_action_player(__Players__[i], &server_window);
    }

    if(__server_ation_id__ == QUIT) {
      wrefresh(server_window.terminal);
      return NULL;
    }

    __server_ation_id__ = NO_ACTION;
    for(int i = 0; i < PLAYERS_MAX; i++){
      __Players__[i]->action_id = NO_ACTION;
    }

    refresh_server(&server_window, __Players__);

    for(int i = 0; i < PLAYERS_MAX; i++){
      if(__Players__[i]->pid > 0 ){
        send_map_to_player(__Players__[i]);
      }
    }
    
    
    usleep(500000);
    __round__++;
  }
  
}

