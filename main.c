#include <ctype.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

#define TRIE_IMPLEMENTATION
#include "trie.h"

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

int term_lines(void) {
    struct winsize w;
    ioctl(0, TIOCGWINSZ, &w);
    return w.ws_row;
}

int find_terminal_words(Trie* start, char* words, int len) {
    if (!(start->children_len) && !(start->terminal)) {
        return len;
    }

    if (start->terminal) {
        if (len + strlen(start->word) > VIABLE_WORD_BUF) {
            return len;
        }

        for (size_t i = 0; i < strlen(start->word); i++) {
            words[len] = start->word[i];
            len++;
        }
        words[len] = '\0';
        len++;
    }

    for (size_t i = 0; i < start->children_len; i++) {
        len = find_terminal_words(start->children[i], words, len);
    }

    return len;
}

char* collate_words(Trie* root, char* prefix) {
    char* viable_words = malloc(VIABLE_WORD_BUF * sizeof(char));
    memset(viable_words, 0, VIABLE_WORD_BUF);

    Trie* start_point = trieSearch(root, prefix);
    if (start_point == NULL) { return viable_words; }

    (void)find_terminal_words(start_point, viable_words, 0);
    return viable_words;
}


void print_words(char* words) {
    printf("\x1b[90m");
    int cur_lines = 0;
    const int lines = term_lines();

    for (int i = 0; i < VIABLE_WORD_BUF; i++) {
        if (words[i] == '\0') { 
            printf("\r\n"); 
            cur_lines++;

            if (cur_lines > lines) { break; }
        } else {
            printf("%c", words[i]);
        }
    }

    printf("\x1b[0m");
}

int main() {
    // Step 1: train the trie on words
    FILE* words = fopen("/usr/share/dict/words", "r");
    if (!words) {
        perror("ERROR: Failed to open words file");
        return -1;
    }

    Trie* root = trieConstruct();

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
    printf("\x1b[2J\x1b[H");

    while(true) {
        printf("\x1b[H\r> %s", input);
        char c = getchar();

        if (c == CTRL_KEY('c')) {
            printf("\x1b[2J\x1b[H");
            break; 
        }

        if (isalpha(c)) {
            input[input_len] = c;
            input_len++;
            input[input_len] = '\0';

            if (input_len >= 3) {
                char* viable_words = collate_words(root, input);
                // TODO: grey colour text
                print_words(viable_words);
                free(viable_words);
            }

            continue;
        }
    }

    disable_raw_mode();
    free(input);
    trieDestruct();
    printf("\n");

    return 0;
}
