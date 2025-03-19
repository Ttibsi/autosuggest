#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <ncurses.h>
#include <ctype.h>

#include "trie.h"

// In C, we need the function sigs in a .c file even if they're inline
extern inline void trieInsert(Trie* t, char c);
extern inline Trie* trieSearch(Trie* t, char* str);
extern inline void trieDelete(Trie* t, char* str);

void draw_options(Trie* root, char* input) {
}

int main() {
    FILE* words = fopen("/usr/share/dict/words", "r");
    // 479826 lines
    if (!words) {
        perror("ERROR: Failed to open words file");
        return -1;
    }

    Trie root = {0};

    char* cur_word = malloc(50 * sizeof(char));
    int word_len = 0;
    char c;

    while ((c = fgetc(words)) != EOF) {
        if (c == '\r') {
            continue;
        } else if (c == '\n') {
            cur_word[word_len] = '\0';
            printf("Trained: %s\n", cur_word);

            cur_word[word_len] = ' ';
            word_len = 0;

        } else {
            // traverse the tree
            trieInsert(trieSearch(&root, cur_word), c);  

            // Add to cur_word for printing purposes
            cur_word[word_len] = c;
            word_len++;
        }
    }

    if (!(feof(words))) { 
        fprintf(stderr, "Error reading words file\n"); 
    }
    
    free(cur_word);
    fclose(words);

    char* input = malloc(50 * sizeof(char));
    input[0] = '\0';
    int input_len = 0;

    keypad(stdscr, TRUE);
    cbreak();
    noecho();

    while(true) {
        printw("\x1b[1K\r> %s\n", input);
        char c = getch();

        if (isalpha(c)) {
            input[input_len] = c;
            input_len++;
            input[input_len] = '\0';
            continue;
        }

        if (input_len >= 3) {
            draw_options(&root, input);

            switch (c) {
                case KEY_UP:
                case KEY_DOWN:
            }
        }
    }

    nocbreak();
    echo();
    return 0;
}
