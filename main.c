#include <ctype.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
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

void collect_words_helper(Trie* node, char* prefix, char* buffer, int depth, char* viable_words, int* word_index) {
    // Base case: stop if node is NULL
    if (node == NULL) return;

    // Add current node's letter to the buffer
    buffer[depth] = node->letter;
    buffer[depth + 1] = '\0';

    // If this is a terminal node (end of a valid word)
    if (node->terminal && depth > 0) {
        // Copy prefix to viable_words
        int prefix_len = strlen(prefix);
        int i = 0;
        while (i < prefix_len - 1 && *word_index < VIABLE_WORD_BUF - 1) {
            viable_words[(*word_index)++] = prefix[i++];
        }

        // Copy current word path (buffer) to viable_words
        i = 0;
        while (buffer[i] != '\0' && *word_index < VIABLE_WORD_BUF - 1) {
            viable_words[(*word_index)++] = buffer[i++];
        }

        // Add word separator
        viable_words[(*word_index)++] = '\0';
    }

    // Recursively explore all children of the current node
    for (size_t i = 0; i < node->children_len; i++) {
        collect_words_helper(node->children[i], prefix, buffer, depth + 1, viable_words, word_index);
    }
}

char* collate_words(Trie* root, char* prefix) {
    char* viable_words = malloc(VIABLE_WORD_BUF * sizeof(char));
    memset(viable_words, 0, VIABLE_WORD_BUF);

    Trie* start_point = trieSearch(root, prefix);
    if (start_point == NULL) { return viable_words; }

    char* buffer = malloc(100 * sizeof(char));
    int word_index = 0;

    // Start collecting words recursively
    collect_words_helper(start_point, prefix, buffer, 0, viable_words, &word_index);

    free(buffer);
    return viable_words;
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
    if (!words) {
        perror("ERROR: Failed to open words file");
        return -1;
    }

    Trie* root = (Trie*)malloc(sizeof(Trie));
    *root = (Trie){
        .children = {NULL},
        .children_len = 0,
        .terminal = true, // Only set terminal at word endings
        .letter = '\0'
    };

    char* cur_word = malloc(50 * sizeof(char));
    cur_word[0] = '\0';
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
            // Add to cur_word for printing purposes
            cur_word[word_len] = c;
            word_len++;
            cur_word[word_len] = '\0';

            // traverse the tree
            Trie* parent = trieSearch(root, cur_word);
            trieInsert((parent == NULL ? root: parent), c);

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
    printf("\x1b[2J\x1b[H");

    while(true) {
        printf("\x1b[H\r> %s", input);
        char c = getchar();

        if (c == CTRL_KEY('c')) { break; }
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
    trieDestroy(root);
    free(input);
    printf("\n");

    return 0;
}
