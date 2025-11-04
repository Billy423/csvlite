# Makefile for CSVlite

CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Werror
INCLUDES = -Iinclude

TARGET = csvlite

# source files will be added (later)
SOURCES = src/main.c
OBJECTS = $(SOURCES:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $^ 

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

test:
	@echo "Tests not yet implemented"

coverage:
	@echo "Coverage not yet implemented"

clean:
	rm -f $(TARGET) $(OBJECTS) *.gcno *.gcda *.gcov

.PHONY: all test coverage clean