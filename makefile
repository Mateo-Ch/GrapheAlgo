CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -I./include
LDFLAGS = -L./lib -lraylib -lm -lgdi32 -lwinmm
SRC = $(wildcard src/*.c)
OBJ = $(SRC:src/%.c=bin/%.o)
BIN_DIR = bin
TARGET = $(BIN_DIR)/graphe

all: $(BIN_DIR) $(TARGET)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

bin/%.o: src/%.c src/%.h
	$(CC) $(CFLAGS) -c $< -o $@

bin/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: all clean