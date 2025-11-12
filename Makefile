# Makefile for CSVlite (extended with a parser demo)

CC        := gcc
CFLAGS    := -std=c11 -Wall -Wextra -Werror
INCLUDES  := -Iinclude

# App target (keep as-is for later)
TARGET    := csvlite
SOURCES   := src/main.c            # add more later: src/csv.c src/cli.c ...
OBJECTS   := $(SOURCES:.c=.o)

# Demo build dirs
BUILD_DIR := build
BIN_DIR   := bin

# -------------------------
# Default: do nothing heavy
# -------------------------
.PHONY: all
all: help

.PHONY: help
help:
	@echo "Useful targets:"
	@echo "  make parse-demo       # build the select_parse_demo"
	@echo "  make run-parse        # build + run the demo"
	@echo "  make $(TARGET)        # build app (when main + deps are ready)"
	@echo "  make clean            # remove build/bin and objects"

# -------------------------
# csvlite app (later)
# -------------------------
$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# -------------------------
# Parser demo (works now)
# -------------------------
# ensure dirs exist
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

# compile select.c to an object (only the parser will be used in the demo)
$(BUILD_DIR)/select.o: src/select.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# link the demo
$(BIN_DIR)/select_parse_demo: $(BUILD_DIR)/select.o tests/unit/select_parse_demo.c | $(BIN_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) $^ -o $@

.PHONY: parse-demo
parse-demo: $(BIN_DIR)/select_parse_demo

.PHONY: run-parse
run-parse: parse-demo
	./$(BIN_DIR)/select_parse_demo

# -------------------------
# Stubs you already had
# -------------------------
.PHONY: test
test:
	@echo "Tests not yet implemented"

.PHONY: coverage
coverage:
	@echo "Coverage not yet implemented"

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR) $(TARGET) $(OBJECTS) *.gcno *.gcda *.gcov
