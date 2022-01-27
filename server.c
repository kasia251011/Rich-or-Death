
#include <sys/ipc.h>
#include <sys/shm.h>

#include <signal.h>
#include <errno.h>
#include <string.h>
#include "server_funs.h"

Shm_game_t * __Shm_game__;
Beast_t __Beasts__[BEAST_MAX];
Window_t __Window__;

void * get_key(void * arg);
void * execute_action(void * arg);

Action_id_t server_action_id = NO_ACTION;

int main(){

  //GET PLAYER FROM SHM
  key_t KEY_SHM = 123;
  int shm_id = shmget(KEY_SHM, sizeof(Shm_game_t), IPC_EXCL | IPC_CREAT | 0666);

  if(errno == EEXIST){
    printf("Server already exist\n");
    return 0;
  }

  __Shm_game__ = (Shm_game_t *)shmat(shm_id, NULL, 0);
  __Shm_game__->round = 0;
  __Shm_game__->server_pid = getpid();
  __Shm_game__->winner_num = -1;

  //init game
  screen_init();
  window_init(&__Window__);
  board_init(&(__Shm_game__->camp_coords));
  players_init(__Shm_game__->Players);

  pthread_t get_key_thread, execute_action_thread;

  //game
  pthread_create(&get_key_thread, NULL, get_key, NULL);
  pthread_create(&execute_action_thread, NULL, execute_action, NULL);

  pthread_join(get_key_thread, NULL);
  pthread_join(execute_action_thread, NULL);


  //clean up
  screen_destroy();
  for(int i = 0; i < PLAYERS_MAX; i++){
    sem_destroy(&__Shm_game__->Players[i].sem_print_map);
  }
  for(int i = 0; i < __beast_counter__; i++){
    sem_destroy(&(__Beasts__[i].semaphore));
  }
  shmdt(__Shm_game__);
  shmctl(shm_id, IPC_RMID, NULL);
  
  return 0;
}



void * get_key(void * arg){
  char action;

  while(1){
    server_action_id = mvwgetch(__Window__.input, 1, 1);
    if(server_action_id == QUIT) return NULL;
  }

  return NULL;
}

void * execute_action(void * arg){

  while (1){

    select_action_server(server_action_id);

    for(int i = 0; i < PLAYERS_MAX; i++){

      //check if player is still online
      if(kill(__Shm_game__->Players[i].pid, 0) != 0){
        if(__Shm_game__->Players[i].pid != NO_PROCESS){
          delete_player(__Shm_game__->Players + i, i);
        }
      }

      select_action_player(__Shm_game__->Players + i);
    }

    push_beasts();

    if(server_action_id == QUIT) {
      wrefresh(__Window__.terminal);
      return NULL;
    }

    server_action_id = NO_ACTION;
    for(int i = 0; i < PLAYERS_MAX; i++){
      __Shm_game__->Players[i].action_id = NO_ACTION;
    }

    refresh_server();

    

    for(int i = 0; i < PLAYERS_MAX; i++){
      if(__Shm_game__->Players[i].pid > 0 ){
        send_map_to_player(&(__Shm_game__->Players[i]));
        sem_post(&(__Shm_game__->Players[i].sem_print_map));
        
      }
    }

    winner();
    if(__Shm_game__->winner_num != -1){
      return end_game(__Shm_game__->winner_num, __Window__.board);
    }
    
    usleep(200000);
    __Shm_game__->round++;
  }
  
}

