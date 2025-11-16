# ===== CSVlite demo Makefile =====
CC        := gcc
CFLAGS    := -std=c11 -Wall -Wextra -Werror
INCLUDES  := -Iinclude

BUILD_DIR := build
BIN_DIR   := bin

# --- default ---
.PHONY: all
all: run-demo

# --- ensure dirs exist ---
$(BUILD_DIR) $(BIN_DIR):
	mkdir -p $@

# --- objects ---
$(BUILD_DIR)/select.o: src/select.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BUILD_DIR)/vec.o: src/vec.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BUILD_DIR)/row.o: src/row.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BUILD_DIR)/hmap.o: src/hmap.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# --- demo linking (parse + project) ---
$(BIN_DIR)/select_demo: \
	$(BUILD_DIR)/select.o \
	$(BUILD_DIR)/vec.o \
	$(BUILD_DIR)/row.o \
	$(BUILD_DIR)/hmap.o \
	tests/unit/select_demo.c | $(BIN_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) $^ -o $@

.PHONY: demo
demo: $(BIN_DIR)/select_demo

.PHONY: run-demo
run-demo: demo
	./$(BIN_DIR)/select_demo

# --- clean ---
.PHONY: clean
clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)
