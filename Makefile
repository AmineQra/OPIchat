CC = gcc
CFLAGS = -Wall -Werror -Wextra -std=c99 -D_DEFAULT_SOURCE=200809L -g -pthread

CLI = opichat_client
SRC_CLI = src/client.c  src/utils/client/notification.c src/utils/client/thread.c
OBJ_CLI = $(SRC_CLI:%.c=%.o)

SERV = opichat_server
SRC_SERV = src/server.c src/utils/server/client_struct.c src/utils/server/command_add.c src/utils/server/command_basic.c src/utils/server/connection.c 
SRC_SERV += src/utils/server/list.c src/utils/server/room.c src/utils/server/server_tools.c
OBJ_SERV = $(SRC_SERV:%.c=%.o)

SRC_UTILS = src/utils/commands.c src/utils/frees.c src/utils/package.c src/utils/parsing.c src/utils/tools.c src/utils/xalloc.c
OBJ_UTILS = $(SRC_UTILS:%.c=%.o)

CFLAGS += -fsanitize=address
LDFLAGS += -fsanitize=address

all: $(CLI) $(SERV)

$(CLI): $(OBJ_CLI) $(OBJ_UTILS)
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $(CLI)

$(SERV): $(OBJ_SERV) $(OBJ_UTILS)
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $(SERV)

check:
	$(CC) $(CFLAGS) tests/check.c $(SRC_UTILS) -o tests/ch
	./tests/ch

clean:
	$(RM) $(OBJ_CLI) $(OBJ_SERV) $(OBJ_UTILS) $(CLI) $(SERV) tests/ch