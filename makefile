all: server player

server: server.c server_funs.c server_funs.h game.c game.h
	gcc server.c game.c server_funs.c -pthread -o server -lncurses

player: game.c game.h player.c player_funs.c player_funs.h
	gcc game.c player.c player_funs.c -pthread -o player -lncurses