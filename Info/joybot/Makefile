CC = gcc
CFLAGS = 
LDFLAGS = 


all: joybot

joybot: joybot.o
	$(CC) -o $@ $^

%.o: %.c
	$(CC) -o $@ -c $^

clean:
	rm -f *.o joybot

