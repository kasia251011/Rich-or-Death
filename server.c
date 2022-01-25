#include <unistd.h> //sleep
#include <sys/ipc.h>
#include <sys/shm.h>
#include <pthread.h>
#include <signal.h>
#include <errno.h>
#include <string.h>
#include "server_funs.h"

Shm_game_t * Shm_game = NULL;

void * get_key(void * arg);
void * execute_action(void * arg);

Window_t server_window;
Action_id_t server_action_id = NO_ACTION;

int main(){

  //GET PLAYER FROM SHM
  key_t KEY_SHM = 123;
  int shm_id = shmget(KEY_SHM, sizeof(Shm_game_t), IPC_CREAT | 0666);
  Shm_game = (Shm_game_t *)shmat(shm_id, NULL, 0);
  

  Shm_game->round = 0;
  

  screen_init();
  window_init(&server_window);
  board_init();
  keypad(server_window.input, true);
  players_init(Shm_game->Players);

  pthread_t get_key_thread, execute_action_thread;

  pthread_create(&get_key_thread, NULL, get_key, NULL);
  pthread_create(&execute_action_thread, NULL, execute_action, NULL);

  pthread_join(get_key_thread, NULL);
  pthread_join(execute_action_thread, NULL);

  screen_destroy();

  shmdt(Shm_game);
  shmctl(shm_id, IPC_RMID, NULL);
  
  return 0;
}



void * get_key(void * arg){
  char action;

  while(1){
    server_action_id = mvwgetch(server_window.input, 1, 1);
    if(server_action_id == QUIT) return NULL;
  }

  return NULL;
}

void * execute_action(void * arg){

  while (1){

    select_action_server(server_action_id, &server_window);

    for(int i = 0; i < PLAYERS_MAX; i++){

      if(kill(Shm_game->Players[i].pid, 0) != 0){
        if(Shm_game->Players[i].pid != NO_PROCESS){
          delete_player(Shm_game->Players + i, i);
        }
      }

      select_action_player(Shm_game->Players + i , &server_window);
    }

    if(server_action_id == QUIT) {
      wrefresh(server_window.terminal);
      return NULL;
    }

    server_action_id = NO_ACTION;
    for(int i = 0; i < PLAYERS_MAX; i++){
      Shm_game->Players[i].action_id = NO_ACTION;
    }

    wrefresh(server_window.board);
    wrefresh(server_window.terminal);
    wrefresh(server_window.stats);

    refresh_server(&server_window, Shm_game->Players, Shm_game->round);

    for(int i = 0; i < PLAYERS_MAX; i++){
      if(Shm_game->Players[i].pid > 0 ){
        send_map_to_player(&(Shm_game->Players[i]));
      }
    }
    
    
    sleep(1);
    Shm_game->round++;
  }
  
}

