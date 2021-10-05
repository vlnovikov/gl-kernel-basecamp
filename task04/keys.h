#pragma once

#define PRESSED 1
#define UNPRESSED 0

#define KEY_1 18
#define KEY_2 23
#define KEY_3 24

void init_key(int id);
int get_key_status(int id);