CC := gcc
CFLAGS := -Wall -Wextra -std=c11 -Wimplicit-fallthrough -g

ifeq (0, 1)
	san := -fsanitize=address,undefined
endif

sources := $(filter-out src/test.c,$(wildcard src/*.c))
objects := $(patsubst src/%.c,build/%.o,$(sources))

test_sources := $(filter-out src/main.c,$(wildcard src/*.c))
test_objects := $(patsubst src/%.c,build/%.o,$(test_sources))

.PHONY: all
all: auto

build:
	mkdir build

build/%.o: src/%.c | build
	$(CC) $< -c -MMD -MP -o $@ $(CFLAGS) $(san)

auto: $(objects)
	$(CC) $^ -o $@ $(san)

test_exe: $(test_objects)
	$(CC) $^ -o $@ $(san)

.PHONY: test
test: test_exe
	./test_exe


.PHONY: clean
clean:
	rm -rf build
	if [ -f auto ]; then rm auto; fi
	if [ -f test_exe ]; then rm test_exe; fi
	if [ -f core ]; then rm core; fi

-include $(objects:.o=.d)
