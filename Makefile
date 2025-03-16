CC := gcc
CFLAGS := -Wall -Wextra -std=c11 -Wimplicit-fallthrough -g -fsanitize=address,undefined
LDFLAGS := 

all: auto

main.o: main.c
	$(CC) $< -c -o $@ $(CFLAGS) $(LDFLAGS)

auto: main.o
	$(CC) $^ -o $@ $(CFLAGS) $(LDFLAGS)

.PHONY: clean
clean:
	rm main.o
	rm auto
