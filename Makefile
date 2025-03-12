CC := gcc
CFLAGS := -Wall -Wextra -std=c11 -Wimplicit-fallthrough -g
LDFLAGS := 

all: auto

main.o: main.c
	$(CC) $< -c -o $@ $(CFLAGS) $(LDFLAGS)

auto: main.o
	$(CC) $^ -o $@ $(CXXFLAGS) $(LDFLAGS)

.PHONY: clean
clean:
	rm main.o
	rm auto
