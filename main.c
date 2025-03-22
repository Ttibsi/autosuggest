#include <ctype.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

#include "trie.h"

#define CTRL_KEY(k) ((k) & 0x1f)
#define VIABLE_WORD_BUF 1024

// In C, we need the function sigs in a .c file even if they're inline
extern inline void trieInsert(Trie* t, char c);
extern inline Trie* trieSearch(Trie* t, char* str);
extern inline void trieDelete(Trie* t, char* str);

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

char* collate_words(Trie* root, char* prefix) {
    char* viable_words = malloc(VIABLE_WORD_BUF * sizeof(char));
    Trie* start_point = trieSearch(root, prefix);
    // for child in children
        // Create new string, add prefix
        // iterate (recurse?) through child's children, 

}

void print_words(char* words) {
    for (int i = 0; i < VIABLE_WORD_BUF; i++) {
        if (words[i] == '\0') { 
            printf("\r\n"); 
        } else {
            printf("%c", words[i]);
        }
    }
}

int main() {
    // Step 1: train the trie on words
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

    // Step 2: Get user input and display suggestions
    char* input = malloc(50 * sizeof(char));
    input[0] = '\0';
    int input_len = 0;

    enable_raw_mode();

    while(true) {
        printf("\x1b[2K\r> %s", input);
        char c = getchar();

        if (c == CTRL_KEY('c')) { break; }
        if (isalpha(c)) {
            input[input_len] = c;
            input_len++;
            input[input_len] = '\0';

            if (input_len >= 3) {
                char* viable_words = collate_words(&root, input);
                // TODO: grey colour text
                print_words(viable_words);
                free(viable_words);
            }

            continue;
        }

    }

    disable_raw_mode();
    free(input);
    printf("\n");

    return 0;
}
