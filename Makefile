PKGS=sdl2 glew
CC=gcc
CFLAGS=-Wall -Wextra -std=c11 -pedantic -ggdb `pkg-config --cflags $(PKGS)`
LIBS=`pkg-config --libs $(PKGS)` -lm
SRC=main.c editor.c file.c glextra.c la.c 
HEADERS=editor.h file.h glextra.h la.h macros.h stbi.h sv.h
NAME=Plexitor

$(NAME): $(SRC) $(HEADERS)
	@echo "compilation of Plexitor ... "
	@$(CC) $(CFLAGS) -o $(NAME) $(SRC) $(LIBS)

all : $(NAME)

clean:
	@echo "clean ... "
	@rm -f *.o

fclean: clean
	@echo "fclean ... "
	@rm -f $(NAME)

re: fclean $(NAME)

.PHONY: all clean fclean re
