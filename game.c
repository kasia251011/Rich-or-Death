#include "game.h"


void screen_init(){
  initscr();
  cbreak();
  curs_set(0);
  //noecho();
  
  start_color();
}

void screen_destroy(){
  getch();
  endwin();
}

void window_init(Window_t * window){
  srand(time(NULL));

  window->board = newwin(BOARD_HEIGHT, BOARD_WIDTH, 1, 1);
  window->stats = newwin(STATS_HEIGHT, STATS_WIDTH, 1, 55);
  window->terminal = newwin(3, BOARD_WIDTH, 27, 1);
  window->input = newwin(3, BOARD_WIDTH, 30, 1);

  keypad(window->input, true);
  refresh();

  box(window->stats, 0, 0);
  box(window->terminal, 0, 0);
  box(window->input, 0, 0);

  mvwprintw(window->stats, 0, 2, "Stats");
  mvwprintw(window->terminal, 0, 2, "Terminal");
  mvwprintw(window->input, 0, 2, "Input");

  //legend
  Blocks_t Blocks;
  blocks_init(&Blocks);

  mvwprintw(window->stats, 14, 1, "Legend: ");
  for(int i = 0; i < PLAYERS_MAX; i++){
    mvwaddch(window->stats, 15, 2 + i, Blocks.Players[i]);
  }
  mvwprintw(window->stats, 15, 6, " - players");

  mvwaddch(window->stats, 16, 2, Blocks.Bushes);
  mvwprintw(window->stats, 16, 6, " - bushes (slow down)");

  mvwaddch(window->stats, 17, 2, Blocks.OneCoin);
  mvwprintw(window->stats, 17, 6, " - one coin");
  
  mvwaddch(window->stats, 18, 2, Blocks.Treasue);
  mvwprintw(window->stats, 18, 6, " - treasure");

  mvwaddch(window->stats, 19, 2, Blocks.Large_Treasure);
  mvwprintw(window->stats, 19, 6, " - large treasure");

  mvwaddch(window->stats, 20, 2, Blocks.Dropped_Treasure);
  mvwprintw(window->stats, 20, 6, " - dropped treasure");

  mvwaddch(window->stats, 21, 2, Blocks.Beast);
  mvwprintw(window->stats, 21, 6, " - wild beas");

  mvwaddch(window->stats, 22, 2, Blocks.Campasite);
  mvwprintw(window->stats, 22, 6, " - campasite");


  wrefresh(window->stats);
  wrefresh(window->terminal);
  wrefresh(window->input);
}

void blocks_init(Blocks_t * blocks){
  init_pair(1, COLOR_WHITE, COLOR_BLUE);
  init_pair(2, COLOR_BLACK, COLOR_BLACK);
  init_pair(3, COLOR_BLACK, COLOR_WHITE);
  init_pair(4, COLOR_BLACK, COLOR_YELLOW);
  init_pair(5, COLOR_RED, COLOR_BLUE);
  init_pair(6, COLOR_YELLOW, COLOR_GREEN);

  blocks->Wall = '|' | COLOR_PAIR(2);
  blocks->Path = ' ' | COLOR_PAIR(3);
  blocks->Bushes = '#' | COLOR_PAIR(3);
  blocks->OneCoin = 'c' | COLOR_PAIR(4);
  blocks->Treasue = 't' | COLOR_PAIR(4);
  blocks->Large_Treasure = 'T' | COLOR_PAIR(4);
  blocks->Beast = '*' | COLOR_PAIR(5);
  blocks->Campasite = 'A' | COLOR_PAIR(6);
  blocks->Dropped_Treasure = 'D' | COLOR_PAIR(4);

  for(int i = 0; i < PLAYERS_MAX; i++){
    blocks->Players[i] = i + 1 + '0' | COLOR_PAIR(1);
  }
}



