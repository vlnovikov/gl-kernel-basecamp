#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <glib.h>
#include <ncurses.h>

#define THREAD_COUNT 4

static int enabled = 1;

struct thread_info {
	size_t n;
	size_t p;
};

static pthread_t info[THREAD_COUNT];
static struct thread_info aux[THREAD_COUNT];
static pthread_attr_t attr;

static pthread_mutex_t mutex;
static pthread_mutex_t print_lock;
static pthread_cond_t cond;

struct block_data {
	char c;
	size_t block_length;
	size_t number_of_blocks;
};

GList *data_list = 0;
WINDOW *window;

void put_char(struct thread_info *i, char c)
{
	size_t y, x;
	pthread_mutex_lock(&print_lock);
	getyx(window, y, x);
	mvaddch(i->n, i->p, c);
	++i->p;
	move(y, x);
	refresh();
	pthread_mutex_unlock(&print_lock);
}

void *runner(void *arg)
{
	struct thread_info *info = arg;
	struct block_data *data;
	while (enabled) {
		pthread_mutex_lock(&mutex);
		if (data_list == 0) {
			pthread_cond_wait(&cond, &mutex);
		}
		if (!enabled) {
			pthread_mutex_unlock(&mutex);
			return 0;
		}
		data = data_list->data;
		data_list = g_list_remove_link(data_list, data_list);
		pthread_mutex_unlock(&mutex);
		size_t i, j;
		for (i = 0; i < data->number_of_blocks; ++i) {
			for (j = 0; j < data->block_length; ++j) {
				put_char(info, data->c);
				sleep(1);
				if (!enabled) {
					return 0;
				}
			}
			put_char(info, ' ');
			sleep(1);
		}
		free(data);
	}
	return 0;
}

static void init_ncurses(void)
{
	window = initscr();
	curs_set(1);
}

static void shutdown_ncurses(void)
{
	refresh();
	endwin();
}

void init_threads()
{
	pthread_mutex_init(&mutex, 0);
	pthread_mutex_init(&print_lock, 0);
	pthread_cond_init(&cond, 0);
	pthread_attr_init(&attr);

	for (size_t i = 0; i < THREAD_COUNT; ++i) {
		aux[i].n = i;
		aux[i].p = 0;
		pthread_create(info + i, &attr, &runner, aux + i);
	}
}

void shutdown_threads()
{
	void *out;
	enabled = 0;

	for (size_t i = 0; i < THREAD_COUNT; ++i) {
		pthread_cond_signal(&cond);
	}
	for (size_t i = 0; i < THREAD_COUNT; ++i) {
		pthread_join(info[i], &out);
	}

	pthread_mutex_destroy(&mutex);
	pthread_mutex_destroy(&print_lock);
	pthread_cond_destroy(&cond);
}

void append_data(char c, size_t l, size_t n)
{
	struct block_data *d = malloc(sizeof(struct block_data));
	d->c = c;
	d->block_length = l;
	d->number_of_blocks = n;

	pthread_mutex_lock(&mutex);
	data_list = g_list_append(data_list, d);
	pthread_mutex_unlock(&mutex);
	pthread_cond_signal(&cond);
}

int main(int argc, char *argv[])
{
	char buffer[256];

	init_threads();
	init_ncurses();

	while (1) {
		char c;
		size_t l, n;
		move(THREAD_COUNT + 1, 0);
		clrtoeol();
		getnstr(buffer, 256);
		if (sscanf(buffer, "%c %zu %zu", &c, &l, &n) == 3) {
			append_data(c, l, n);
		} else {
			break;
		}
	}

	shutdown_threads();
	shutdown_ncurses();
	return 0;
}