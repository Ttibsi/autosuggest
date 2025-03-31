CC := gcc
CFLAGS := -Wall -Wextra -std=c11 -Wimplicit-fallthrough -g -fsanitize=address,undefined

all: auto

main.o: main.c
	$(CC) $< -c -o $@ -MMD -MP $(CFLAGS)

auto: main.o 
	$(CC) $^ -o $@ $(CFLAGS)

test.o: test.c
	$(CC) $< -c -o $@ -MMD -MP $(CFLAGS)

test: test.o
	$(CC) $^ -o $@ $(CFLAGS)
	./test

.PHONY: clean
clean:
	rm main.o
	rm test.o
	rm auto
	rm test
	rm *.d 

-include main.d
-include test.d
