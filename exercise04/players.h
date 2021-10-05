#ifndef PLAYERS_H
#define PLAYERS_H

#include <glib.h>

#define MAX_PLAYERS 8

typedef struct {
	char *name;
	unsigned int win_count;
	int current_roll[2];
} Player;

GList *create_player_list(int count);
void destroy_player_list(GList *players);
void roll_round(GList *players);

#endif