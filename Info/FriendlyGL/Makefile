CC=agcc
CFLAGS=-W -Wall -Iinclude -lpthread
LDFLAGS=-L lib -l freetype -l ts -lpthread

all: bin/gui

bin/gui: obj/gui.o obj/fb.o obj/ft.o obj/ts.o
	$(CC) $(LDFLAGS) -o $@ $^

obj/%.o: src/%.c include/%.h
	$(CC) $(CFLAGS) -o $@ -c $<

clean:
	rm obj/*.o

mrproper: clean
	rm bin/*
