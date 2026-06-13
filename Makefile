CC=gcc
SRC_DIR=./src
BIN_DIR=./bin
OBJ_DIR=./obj
INCLUDE_DIRS=-I$(SRC_DIR)
LIBS=-lsqlite3
BIN=$(BIN_DIR)/wg-quota
SRCS=$(wildcard $(SRC_DIR)/*.c)
OBJS=$(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRCS))

.PHONY: mkdir all

all: mkdir $(OBJS)
	$(CC) $(INCLUDE_DIRS) $(OBJS) $(LIBS) -o $(BIN)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(INCLUDE_DIRS) -c $^ $(LIBS) -o $@

mkdir:
	@if [ ! -d bin ]; then mkdir bin; fi
	@if [ ! -d obj ]; then mkdir obj; fi

clean:
	@rm bin -r
	@rm obj -r
