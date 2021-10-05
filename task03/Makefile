all: pack upack

pack:
	$(CC) $(CFLAGS) encoder.c -o pack

upack:
	$(CC) $(CFLAGS) decoder.c -o upack

clean:
	rm -rf pack upack task03 task03.deb

install: all
	mkdir -p task03/opt/task03/bin
	cp pack upack test.sh task03/opt/task03/bin
	mkdir -p task03/opt/task03/src
	cp encoder.c decoder.c task03/opt/task03/src
	mkdir -p task03/DEBIAN
	cp control task03/DEBIAN
	dpkg --build task03

.PHONY: all pack upack