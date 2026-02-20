CC = gcc
CFLAGS = -Wall -Wextra -std=c99
SRC = src/*
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

bin/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: all clean