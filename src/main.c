#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

#include "dll.h"
#include "trie.h"

#define ARENA_IMPLEMENTATION
#include "arena.h"

#define CTRL_KEY(k) ((k) & 0x1f)
#define VIABLE_WORD_BUF 1024

struct termios orig_termios;

void enable_raw_mode(void) {
  tcgetattr(STDIN_FILENO, &orig_termios);

  struct termios raw = orig_termios;
  raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
  raw.c_oflag &= ~(OPOST);
  raw.c_cflag |= (CS8);
  raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);

  tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

void disable_raw_mode(void) {
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

size_t term_lines(void) {
  struct winsize w;
  ioctl(0, TIOCGWINSZ, &w);
  return w.ws_row;
}

void clear_screen(void) { printf("\x1b[2J\x1b[H"); }

void clear_line(void) { printf("\x1b[2K\r"); }

void find_terminal_words(Trie *start, Node *words, Arena *dll_arena,
                         int extant_lines) {
  if (start == NULL) {
    return;
  }
  if (nodeLen(words) >= term_lines() - extant_lines) {
    return;
  }

  if (!(start->children_len) && !(start->terminal)) {
    return;
  }

  if (start->terminal) {
    Node n = nodeCreate(start->word, dll_arena);
    Node *parent = words;
    while (parent->next != NULL) {
      parent = parent->next;
    }
    n.prev = parent;

    parent->next = arena_alloc(dll_arena, sizeof(Node));
    memcpy(parent->next, &n, sizeof(Node));
  }

  for (size_t i = 0; i < start->children_len; i++) {
    find_terminal_words(start->children[i], words, dll_arena, extant_lines);
  }
}

void collate_words(Trie *root, char *prefix, Node *viable_words,
                   Arena *dll_arena, int extant_lines) {
  Trie *start_point = trieSearch(root, prefix);
  if (start_point == NULL) {
    return;
  }

  find_terminal_words(start_point, viable_words, dll_arena, extant_lines);
}

void print_words(Node *words) {
  printf("\r\x1b[90m");
  while (words->next != NULL) {

    if (words->selected) {
      printf("\x1b[7m");
    }
    printf("%s\r\n", words->word);
    if (words->selected) {
      printf("\x1b[27m");
    }

    words = words->next;
  }
  printf("\x1b[0m");
}

Node *select_next(Node *words) {
  Node *start = words;
  while (words->next != NULL) {
    if (words->selected) {
      words->next->selected = true;
      words->selected = false;
      return start;
    }

    words = words->next;
  }

  return start;
}

Node *select_prev(Node *words) {
  Node *start = words;
  while (words->next != NULL) {
    if (words->selected) {
      words->prev->selected = true;
      words->selected = false;
      return start;
    }

    words = words->next;
  }

  return start;
}

void select_current_word(Node *viable, Node *selected, Arena *dll_arena) {
  while (!viable->selected) {
    viable = viable->next;
  }
  while (selected->next != NULL) {
    selected = selected->next;
  }

  Node n = nodeCreate("", dll_arena);
  n.word = arena_alloc(dll_arena, sizeof(char) * (strlen(viable->word) + 1));
  n.word = strcpy(n.word, viable->word);
  n.prev = selected;

  selected->next = arena_alloc(dll_arena, sizeof(Node));
  memcpy(selected->next, &n, sizeof(Node));
}

void print_selected_words(Node *selected) {
  while (selected->next != NULL) {
    selected = selected->next;
    printf("$ %s\r\n", selected->word);
  }
}

void generate_viable_words(Node *viable_words, Trie *root, char *input,
                           Arena *dll_arena, int selected_len,
                           Node *selected_words) {
  clear_screen();
  print_selected_words(selected_words);
  printf("\x1b[%d;1H\r> %s", selected_len, input);

  collate_words(root, input, viable_words, dll_arena, selected_len);
  if (viable_words->next != NULL) {
    viable_words->next->selected = true;
    print_words(viable_words);
  }
}

int main() {
  // Step 1: train the trie on words
  FILE *words = fopen("/usr/share/dict/words", "r");
  if (!words) {
    perror("ERROR: Failed to open words file");
    return -1;
  }

  Arena trie_arena = {0};
  Trie *root = trieConstruct(&trie_arena);
  Trie *current_node = root;

  char *cur_word = malloc(50 * sizeof(char));
  cur_word[0] = '\0';
  int word_len = 0;
  int c; // This has to be an int for the sake of EOF comparison
  int word_count = 0;

  while ((c = fgetc(words)) != EOF) {
    if (c == '\r') {
      continue;
    } else if (c == '\n') {
      current_node->terminal = true;

      word_count++;
      printf("Trained (%i): %s\n", word_count, cur_word);

      cur_word[0] = '\0';
      word_len = 0;
      current_node = root;

    } else {
      // traverse the tree
      current_node = trieInsert(current_node, c, false, &trie_arena);

      // Add to cur_word for printing purposes
      cur_word[word_len] = c;
      cur_word[++word_len] = '\0';
    }
  }

  free(cur_word);
  fclose(words);

  // Step 2: Get user input and display suggestions
  char *input = malloc(50 * sizeof(char));
  input[0] = '\0';
  int input_len = 0;

  enable_raw_mode();
  clear_screen();

  Arena dll_arena = {0};
  Node viable_words = nodeCreate("", &dll_arena);
  Node selected_words = nodeCreate("", &dll_arena);

  while (true) {
    clear_screen();

    if (selected_words.next != NULL) {
      print_selected_words(&selected_words);
    }

    printf("> %s", input);

    if (viable_words.next != NULL) {
      print_words(&viable_words);
    }

    char c = getchar();
    if (c == CTRL_KEY('c')) {
      clear_screen();
      break;

    } else if (c == CTRL_KEY('n')) {
      viable_words = *select_next(&viable_words);
      clear_screen();
      print_words(&viable_words);
    } else if (c == CTRL_KEY('p')) {
      viable_words = *select_prev(&viable_words);
      clear_screen();
      print_words(&viable_words);
    } else if (c == CTRL_KEY('y')) {
      select_current_word(&viable_words, &selected_words, &dll_arena);
      input_len = 0;
      input[0] = '\0';

      viable_words = nodeCreate("", &dll_arena);
    } else if (c == 0x7F) { // Backspace
      if (input_len > 0) {
        input[--input_len] = '\0';

        if (input_len >= 3) {
          generate_viable_words(&viable_words, root, input, &dll_arena,
                                nodeLen(&selected_words), &selected_words);
        } else {
          clear_line();
        }
      }

    } else if (isalpha(c)) {
      printf("%c", c);
      input[input_len] = c;
      input_len++;
      input[input_len] = '\0';

      if (input_len >= 3) {
        viable_words = nodeCreate("", &dll_arena);

        generate_viable_words(&viable_words, root, input, &dll_arena,
                              nodeLen(&selected_words), &selected_words);
      }
    }
  }

  disable_raw_mode();
  free(input);
  trieDestruct(&trie_arena);

  arena_free(&dll_arena);

  return 0;
}
