#include "players.h"

#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

int roll_sum(const int r[2])
{
	return r[0] + r[1];
}

void print_players_rolls(gpointer player, gpointer user_data)
{
	g_usleep(100000);
	Player *p = player;
	static const char *faces[] = { "\u2680", "\u2681", "\u2682",
				       "\u2683", "\u2684", "\u2685" };
	int *rolls = p->current_roll;
	if (faces[0] && faces[1]) {
		printf("%s (%u): %s %s\n", p->name, p->win_count,
		       faces[rolls[0] - 1], faces[rolls[1] - 1]);
	}
}

gint sort_players_by_rolls(gconstpointer a, gconstpointer b)
{
	const int *v1 = ((const Player *)a)->current_roll;
	const int *v2 = ((const Player *)b)->current_roll;
	return roll_sum(v2) - roll_sum(v1);
}

gint sort_players_by_wins(gconstpointer a, gconstpointer b)
{
	return ((const Player *)b)->win_count - ((const Player *)a)->win_count;
}

void update_leaderboard(gpointer player, gpointer max_score)
{
	int m = *(int *)(max_score);
	Player *p = player;
	int win_count = roll_sum(p->current_roll) == *(int *)max_score ? 1 : 0;
	p->win_count += win_count;
	if (win_count) {
		printf("%s ", p->name);
	}
}

int main(int argc, char *argv[])
{
	int players_count = 4;
	if (argc >= 2) {
		players_count = atoi(argv[1]);
	}

	GList *players = create_player_list(players_count);

	do {
		printf("Rolling...\n");
		roll_round(players);
		g_list_foreach(players, print_players_rolls, NULL);

		printf("The winners: ");
		players = g_list_sort(players, sort_players_by_rolls);
		int max_score =
			roll_sum(((Player *)players->data)->current_roll);
		g_list_foreach(players, update_leaderboard, &max_score);
		printf("\n");
		players = g_list_sort(players, sort_players_by_wins);
	} while (getc(stdin) != 'q');

	destroy_player_list(players);
	return 0;
}