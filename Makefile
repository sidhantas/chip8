CC=/opt/homebrew/opt/llvm/bin/clang
CFLAGS=-g -Wall -Werror -w -D_XOPEN_SOURCE_EXTENDED -I/opt/homebrew/Cellar/argp-standalone/1.3/include
LDFLAGS=-lncurses -framework ApplicationServices -framework Carbon -pthread -L/opt/homebrew/Cellar/argp-standalone/1.3/lib
SRC_DIR := src
OBJ_DIR := obj
BIN_DIR := .
EXE := $(BIN_DIR)/chip8
TEST := $(BIN_DIR)/chip8_test
SRC := $(filter-out src/chip8.c src/tests.c, $(wildcard $(SRC_DIR)/*.c))
OBJ := $(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
EXE_SRC = $(SRC_DIR)/chip8.c
EXE_OBJ := $(OBJ_DIR)/chip8.o
TEST_SRC = $(SRC_DIR)/tests.c
TEST_OBJ = $(OBJ_DIR)/tests.o
.PHONY: all

all: $(EXE)

test: $(TEST)

$(EXE): $(OBJ) $(EXE_OBJ) | $(BIN_DIR)
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

$(TEST): $(OBJ) $(TEST_OBJ) | $(BIN_DIR)
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BIN_DIR) $(OBJ_DIR):
	mkdir -p $@

$(EXE_OBJ): $(EXE_SRC)
	$(CC) $(CFLAGS) -c $< -o $@

$(TEST_OBJ): $(TEST_SRC)
	$(CC) $(CFLAGS) -c $< -o $@

-include $(OBJ.o=.d)

clean:
	rm -rf $(OBJ_DIR) $(EXE) $(TEST)
