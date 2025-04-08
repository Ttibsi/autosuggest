CC := gcc
CFLAGS := -Wall -Wextra -std=c11 -Wimplicit-fallthrough -g 
#-fsanitize=address,undefined

OBJ: main.o trie.o

all: auto

%.o: %.c
	$(CC) $< -c -o $@ -MMD -MP $(CFLAGS)

auto: $(OBJ)
	$(CC) $^ -o $@ $(CFLAGS)

test: test.o $(OBJ)
	$(CC) $^ -o $@ $(CFLAGS)

.PHONY: t
t: test
	./test

.PHONY: clean
clean:
	rm *.o
	rm *.d 
	rm auto test

-include $(OBJ:.o=.d)
