PKGS=sdl2 glew
CC=gcc
CFLAGS=-Wall -Wextra -std=c11 -pedantic -ggdb `pkg-config --cflags $(PKGS)`
LIBS=`pkg-config --libs $(PKGS)` -lm
SRC=$(wildcard *.c)

Plexitor:
	@echo "compilation of Plexitor ... "
	@$(CC) $(CFLAGS) -o Plexitor $(SRC) $(LIBS)
