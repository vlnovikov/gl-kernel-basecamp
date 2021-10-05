
ifdef CHECK_LEAKS
	CC=clang
	CFLAGS+=-g -fsanitize=address
	LD_FLAGS+=-lasan
endif

ifneq ($(shell $(CC) --version | grep -c GCC),0)
	CFLAGS+=-fanalyzer
endif

LIBS=glib-2.0
CFLAGS+=$(foreach library, $(LIBS), $(shell pkg-config --cflags $(library)))
LD_FLAGS+=$(foreach library, $(LIBS), $(shell pkg-config --libs $(library)))

all: dice tests

dice: dice.o players.a
	$(CC) $(LD_FLAGS) dice.o -L./ -lplayers -o dice

tests: tests.o players.a
	$(CC) $(LD_FLAGS) tests.o -L./ -lplayers -o tests

dice.o:
	$(CC) $(CFLAGS) -c dice.c -o dice.o

tests.o:
	$(CC) $(CFLAGS) -c tests.c -o tests.o

players.o:
	$(CC) $(CFLAGS) -c players.c -o players.o

players.a: players.o
	$(AR) -r libplayers.a players.o

clean:
	rm -f dice.o dice players.o libplayers.a tests.o tests

.PHONY : all clean dice.o players.a players.o libplayers.a tests.o