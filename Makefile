# Makefile
CC = gcc
CFLAGS = -Iinclude -Wall `pkg-config --cflags gtk+-3.0`
LIBS = `pkg-config --libs gtk+-3.0` -lrt -pthread

SRC = src/model.c src/controller.c src/view.c src/main.c
OBJ = $(SRC:.c=.o)

all: multi-shell

multi-shell: $(OBJ)
	$(CC) -o $@ $^ $(LIBS)

clean:
	rm -f $(OBJ) multi-shell
