CC = gcc
CFLAGS = -Wall -Wextra -std=c99
SRC = src/graphe.c src/utils.c src/solveur.c
OBJ = $(SRC:src/%.c=bin/%.o)
BIN_DIR = bin
TARGET = $(BIN_DIR)/graphe

all: $(BIN_DIR) $(TARGET)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

bin/%.o: src/%.c src/%.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: all clean