#include <ctype.h>
#include <stdio.h>
#include <stdbool.h>
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

void clear_screen(void) {
    printf("\x1b[2J\x1b[H");
}

void find_terminal_words(Trie* start, Node* words, Arena* dll_arena) {
    if (start == NULL) { return; }
    if (nodeLen(words) >= term_lines()) { return; }

    if (!(start->children_len) && !(start->terminal)) {
        return;
    }

    if (start->terminal) {
        Node n = nodeCreate(start->word, dll_arena);
        Node* parent = words;
        while (parent->next != NULL) { parent = parent->next; }
        parent->next = malloc(sizeof(Node));
        memcpy(parent->next, &n, sizeof(Node));
        n.prev = parent;
    }

    for (size_t i = 0; i < start->children_len; i++) {
        find_terminal_words(start->children[i], words, dll_arena);
    }
}

void collate_words(Trie* root, char* prefix, Node* viable_words, Arena* dll_arena) {
    Trie* start_point = trieSearch(root, prefix);
    if (start_point == NULL) { return; }

    find_terminal_words(start_point, viable_words, dll_arena);
}

void print_words(Node* words) {
    printf("\r\n\x1b[90m");
    while (words->next != NULL) {

        if (words->selected) { printf("\x1b[7m"); }
        printf("%s\r\n", words->word);
        if (words->selected) { printf("\x1b[27m"); }

        words = words->next;
    }
    printf("\x1b[0m");
}

void select_next(Node* words) {
    while (words->next != NULL) {
        if (words->selected) {
            words->next->selected = true;
            words->selected = false;
            return;
        }

        words = words->next;
    }
}

void select_prev(Node* words) {
    while (words->next != NULL) {
        if (words->selected) {
            words->prev->selected = true;
            words->selected = false;
            return;
        }

        words = words->next;
    }
}

void select_current_word(Node* viable, Node* selected) {
    (void)viable;
    (void)selected;
}

int main() {
    // Step 1: train the trie on words
    FILE* words = fopen("/usr/share/dict/words", "r");
    if (!words) {
        perror("ERROR: Failed to open words file");
        return -1;
    }

    Arena trie_arena = {0};
    Trie* root = trieConstruct(&trie_arena);

    char* cur_word = malloc(50 * sizeof(char));
    cur_word[0] = '\0';
    int word_len = 0;
    int c; // This has to be an int for the sake of EOF comparison
    int word_count = 0;

    while ((c = fgetc(words)) != EOF) {
        if (c == '\r') {
            continue;
        } else if (c == '\n') {
            Trie* parent = trieSearch(root, cur_word);
            parent->terminal = true;

            word_count++;
            printf("Trained (%i): %s\n", word_count, cur_word);

            cur_word[0] = '\0';
            word_len = 0;

        } else {
            // traverse the tree
            Trie* parent = trieSearch(root, cur_word);
            trieInsert((parent == NULL ? root : parent), c, false);

            // Add to cur_word for printing purposes
            cur_word[word_len] = c;
            word_len++;
            cur_word[word_len] = '\0';
        }
    }

    free(cur_word);
    fclose(words);

    // Step 2: Get user input and display suggestions
    char* input = malloc(50 * sizeof(char));
    input[0] = '\0';
    int input_len = 0;

    enable_raw_mode();
    clear_screen();

    Arena dll_arena = {0};
    Node viable_words = nodeCreate("", &dll_arena);
    Node selected_words = nodeCreate("", &dll_arena);

    while(true) {
        printf("\x1b[H\r> %s", input);
        char c = getchar();

        if (c == CTRL_KEY('c')) {
            clear_screen();
            break; 

        } else if (c == CTRL_KEY('n')) {
            select_next(&viable_words);
            print_words(&viable_words);
        } else if (c == CTRL_KEY('p')) {
            select_prev(&viable_words);
            print_words(&viable_words);
        } else if (c == CTRL_KEY('y')) {
            select_current_word(&viable_words, &selected_words);
        } else if (isalpha(c)) {
            printf("%c", c);
            input[input_len] = c;
            input_len++;
            input[input_len] = '\0';

            if (input_len >= 3) {
                collate_words(root, input, &viable_words, &dll_arena);
                viable_words.selected = true;
                print_words(&viable_words);
            }
        }
    }

    disable_raw_mode();
    free(input);
    trieDestruct(&trie_arena);
    nodesFree(viable_words, &dll_arena);

    return 0;
}
