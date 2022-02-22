CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -Werror -pedantic -D_DEFAULT_SOURCE=200809L

basic_server: basic_server.c
	$(CC) $(CFLAGS) $^ -o basic_server

clean:
	rm -f basic_server