#include "players.h"

#define MAX_ROLL 6

const char *CHARACTERS[] = { "Winnie",	"Piglet", "Tigger", "Rabbit",
			     "Eyeoure", "Kanga",  "Roo",    "Owl" };

int roll_dice()
{
	return (rand() % MAX_ROLL) + 1;
}

Player *create_player(const char *name)
{
	Player *p = g_malloc(sizeof(Player));
	*p = (Player){ .name = strdup(name),
		       .win_count = 0,
		       .current_roll = { 0 } };
	return p;
}

void destroy_player(gpointer p, gpointer user_data)
{
	Player *player = (Player *)p;
	free(player->name);
	g_free(p);
}

GList *create_player_list(int count)
{
	if (count > MAX_PLAYERS) {
		return NULL;
	}
	GList *l = NULL;
	for (int i = 0; i < count; ++i) {
		l = g_list_append(l, create_player(CHARACTERS[i]));
	}
	return l;
}

void destroy_player_list(GList *players)
{
	g_list_foreach(players, destroy_player, NULL);
	g_list_free(players);
}

void roll_players_dice(gpointer player, gpointer user_data)
{
	Player *p = player;
	p->current_roll[0] = roll_dice();
	p->current_roll[1] = roll_dice();
}

void roll_round(GList *players)
{
	g_list_foreach(players, roll_players_dice, NULL);
}