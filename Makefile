CC := gcc
CFLAGS := -Wall -Wextra -std=c11 -Wimplicit-fallthrough -g -fsanitize=address,undefined

all: auto

main.o: main.c
	$(CC) $< -c -o $@ $(CFLAGS)

auto: main.o
	$(CC) $^ -o $@ $(CFLAGS)

.PHONY: clean
clean:
	rm main.o
	rm auto
