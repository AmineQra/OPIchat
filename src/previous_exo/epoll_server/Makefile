CC = gcc
CFLAGS = -Wall -Werror -Wextra -std=c99 -D_DEFAULT_SOURCE=200809L

FILE = epoll_server
SRC = epoll-server.c connection.c utils/xalloc.c
OBJ = $(SRC:%.c=%.o)

epoll_server: $(OBJ)
	$(CC) $(CFLAGS) $^ -o $(FILE)
	$(RM) $(OBJ)

clean:
	$(RM) $(OBJ) $(FILE)
