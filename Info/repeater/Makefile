EXEC = main

CC = gcc
LD = gcc

CFLAGS = -std=gnu99 -Iinclude
LDFLAGS = -lrt
DEBUG = -g
WARNING = -Wall -pedantic
OPTIMIZE = -O3

SRC = $(wildcard src/*.c)
OBJ = $(SRC:.c=.o)

all: $(EXEC)

$(EXEC): $(OBJ)
	$(LD) $(DEBUG) $(WARNING) $(LDFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) $(OPTIMIZE) $(DEBUG) $(WARNING) -o $@ -c $^

clean:
	rm -f $(OBJ)

mrproper: clean
	rm -f $(EXEC)
	rm -f gmon.out

.PHONY: clean mrproper
