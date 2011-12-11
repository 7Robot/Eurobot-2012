EXEC = main

CC = gcc
CPP = g++
LD = g++


CFLAGS = -std=gnu99 -Iinclude
CPPFLAGS = -Iinclude
LDFLAGS = -lrt
DEBUG = -g
WARNING = -Wall -pedantic
OPTIMIZE = -O3

SRC = $(wildcard src/*.cpp src/*.c)
TMP = $(SRC:.cpp=.o)
OBJ = $(TMP:.c=.o)
#EXEC = $(OBJ:.o=.bin)

all: $(EXEC)

#%.bin: %.o
$(EXEC): $(OBJ)
	$(LD) $(DEBUG) $(WARNING) $(LDFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) $(OPTIMIZE) $(DEBUG) $(WARNING) -o $@ -c $^

%.o: %.cpp
	$(CPP) $(CPPFLAGS) $(OPTIMIZE) $(DEBUG) $(WARNING) -o $@ -c $^

clean:
	rm -f $(OBJ)

mrproper: clean
	rm -f $(EXEC)
	rm -f gmon.out

.PHONY: clean mrproper
