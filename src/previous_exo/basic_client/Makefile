CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -Werror -pedantic -D_DEFAULT_SOURCE=200809L

basic_client: basic_client.c
	$(CC) $(CFLAGS) $^ -o basic_client

clean:
	rm -f basic_client