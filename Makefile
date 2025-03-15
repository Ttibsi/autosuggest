CC := gcc
CFLAGS := -Wall -Wextra -std=c11 -Wimplicit-fallthrough -g
LDFLAGS := 

all: auto

trie.o: trie.c
	$(CC) $< -c -o $@ $(CFLAGS) $(LDFLAGS)

main.o: main.c
	$(CC) $< -c -o $@ $(CFLAGS) $(LDFLAGS)

auto: main.o trie.o
	$(CC) $^ -o $@ $(CXXFLAGS) $(LDFLAGS)

.PHONY: clean
clean:
	rm main.o
	rm tre.o
	rm auto
