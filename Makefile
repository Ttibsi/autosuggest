CC := gcc
CFLAGS := -Wall -Wextra -std=c11 -Wimplicit-fallthrough -g 
#-fsanitize=address,undefined

all: auto

main.o: main.c
	$(CC) $< -c -o $@ -MMD -MP $(CFLAGS)

auto: main.o 
	$(CC) $^ -o $@ $(CFLAGS)

test.o: test.c
	$(CC) $< -c -o $@ -MMD -MP $(CFLAGS)

test: test.o
	$(CC) $^ -o $@ $(CFLAGS)

.PHONY: t
t: test
	./test

.PHONY: clean
clean:
	rm *.o
	rm *.d 
	rm auto test

-include main.d
-include test.d
