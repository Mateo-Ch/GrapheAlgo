CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -I./include

ifeq ($(OS),Windows_NT)
	RAYLIB_LIB = lib/libraylib_windows.a
	LDFLAGS = -L./lib $(RAYLIB_LIB) -lm -lgdi32 -lwinmm
	RM = del /Q
	MKDIR = if not exist $(BIN_DIR) mkdir $(BIN_DIR)
else
	RAYLIB_LIB = lib/libraylib_linux.a
	LDFLAGS = $(RAYLIB_LIB) -lm -lpthread -ldl
	RM = rm -f
	MKDIR = mkdir -p $(BIN_DIR)
endif

SRC = $(wildcard src/*.c)
OBJ = $(SRC:src/%.c=bin/%.o)
BIN_DIR = bin
TARGET = $(BIN_DIR)/graphe

all: $(BIN_DIR) $(TARGET)

$(BIN_DIR):
	$(MKDIR)

$(TARGET): $(OBJ) $(RAYLIB_LIB)
	$(CC) $(CFLAGS) -o $@ $(OBJ) $(LDFLAGS)

bin/%.o: src/%.c src/%.h
	$(CC) $(CFLAGS) -c $< -o $@

bin/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) $(OBJ) $(TARGET)

.PHONY: all clean