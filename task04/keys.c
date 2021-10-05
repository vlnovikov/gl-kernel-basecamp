#include "keys.h"
#include "c_gpio.h"

void init_key(int id)
{
	setup_gpio(id, INPUT, PUD_UP);
}

int get_key_status(int id)
{
	return !input_gpio(id);
}