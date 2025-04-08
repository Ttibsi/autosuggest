CC := gcc
CFLAGS := -Wall -Wextra -std=c11 -Wimplicit-fallthrough -g 

ifeq (1, 0)
	san := -fsanitize=address,undefined
endif

sources := $(filter-out src/test.c,$(wildcard src/*.c))
objects := $(patsubst src/%.c,build/%.o,$(sources))

test_sources := $(filter-out src/main.c,$(wildcard src/*.c))
test_objects := $(patsubst src/%.c,build/%.o,$(test_sources))

all: auto

build:
	mkdir build

build/%.o: src/%.c | build
	$(CC) $< -c -MMD -MP -o $@ $(CFLAGS) $(san)

auto: $(objects)
	$(CC) $^ -o $@ $(san)

test: $(test_objects)
	$(CC) $^ -o $@


.PHONY: clean
clean:
	rm -rf build 
	if [ -f auto ]; then rm auto; fi
	if [ -f test ]; then rm test; fi
	if [ -f core ]; then rm core; fi

-include $(objects:.o=.d)
