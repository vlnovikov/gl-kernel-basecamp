LIBS=ncurses glib-2.0
CFLAGS+=$(foreach library, $(LIBS), $(shell pkg-config --cflags $(library)))
LD_FLAGS+=$(foreach library, $(LIBS), $(shell pkg-config --libs $(library)))

all:
	$(CC) -o threadpool threadpool.c $(CFLAGS) $(LD_FLAGS) -lpthread
clean:
	rm threadpool