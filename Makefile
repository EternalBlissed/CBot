# Compiler and compiler flags
CC = gcc
CFLAGS = -Wall -Wextra -Wunused-parameter -I. -I./concord/include
LDFLAGS = -L./concord/lib -ldiscord -lpthread -lm -lcurl

# Directories
SRC_DIR = src
BUILD_DIR = build
OUT_DIR = out

# Source files and object files (only main.c)
SRC_FILES = $(SRC_DIR)/main.c
OBJ_FILES = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRC_FILES))

# Output binary
TARGET = $(OUT_DIR)/your_bot

.PHONY: all clean

all: $(BUILD_DIR) $(OUT_DIR) $(TARGET)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJ_FILES)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(OUT_DIR):
	mkdir -p $(OUT_DIR)

clean:
	rm -rf $(BUILD_DIR) $(OUT_DIR)
