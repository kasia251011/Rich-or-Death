
#include <pthread.h>
#include <string.h>


#include "player_funs.h"

Window_t player_window;
Player_t * __Player__;
int * __Menager__;

int __index__ = 0;

void * get_key(void * arg);
void * show_map(void * arg);

int main(){

  //SHM
  if(shm_load_menager(&__Menager__)){
    printf("First server must working\n");
    return 0;
  }

  for(int i = 0; i < PLAYERS_MAX; i++){
    printf("Menager : %d\n", __Menager__[i]);
  }

  if(shm_load_player(&__Player__, __Menager__)){
    printf("Too many players in game\n");
    return 0;
  }

  mvwprintw(player_window.terminal, 1, 1, "PID: %d", __Player__->pid);

  __Player__->pid = getpid();

   mvwprintw(player_window.terminal, 1, 1, "PID: %d", __Player__->pid);
   wrefresh(player_window.terminal);

  screen_init();
  window_init(&player_window);
  keypad(player_window.input, true);

  pthread_t get_key_thread, show_map_thread;

  pthread_create(&get_key_thread, NULL, get_key, NULL);
  //pthread_create(&show_map_thread, NULL, show_map, NULL);

  pthread_join(get_key_thread, NULL);

  __Player__->pid = -3;

  screen_destroy();

  shm_close(__Player__, __Menager__);
  
  return 0;
}

void * get_key(void * arg){
  char action;

  while(1){
    __Player__->action_id = mvwgetch(player_window.input, 1, 1);   

    if(__Player__->action_id == QUIT) break;
  }

  return NULL;
}

void * show_map(void * arg){
  while(1){
    render_map(__Player__, &player_window);
  }
}