#include "players.h"

#include <stdio.h>
#include <math.h>

typedef struct {
	size_t index;
	double *values;
} Rolls;

void add_roll(gpointer player, gpointer user_data)
{
	Player *p = player;
	Rolls *r = user_data;
	r->values[r->index] += p->current_roll[0] + p->current_roll[1];
	++r->index;
}

int test_rolls(int players_count)
{
	int result = 0;
	GList *l = create_player_list(players_count);
	Rolls rolls;
	rolls.values = (double *)calloc(players_count, sizeof(double));
	const int roll_count = 10000;
	for (int i = 0; i < roll_count; ++i) {
		roll_round(l);
		rolls.index = 0;
		g_list_foreach(l, add_roll, &rolls);
	}
	for (int i = 0; i < players_count; ++i) {
		double v = fabs(rolls.values[i] / roll_count - 7.0);
		if (v > 0.1) {
			result = 1;
		}
	}
	free(rolls.values);
	destroy_player_list(l);
	return result;
}

int main(int argc, char *argv[])
{
	int players_count = 4;
	if (argc >= 2) {
		players_count = atoi(argv[1]);
	}
	int tr_result = test_rolls(players_count);
	char *results[] = { "success", "failed" };
	printf("Testing test_rolls: %s\n", results[tr_result]);
	return 0;
}