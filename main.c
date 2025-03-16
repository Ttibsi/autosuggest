#include <stdio.h>
#include <stdlib.h>

#include "trie.h"

// In C, we need the function sigs in a .c file even if they're inline
extern inline void trieInsert(Trie* t, char c);
extern inline Trie* trieSearch(Trie* t, char* str);
extern inline void trieDelete(Trie* t, char* str);

int main() {
    FILE* words = fopen("/usr/share/dict/words", "r");
    // 479826 lines
    if (!words) {
        perror("ERROR: Failed to open words file");
        return -1;
    }

    Trie root = {0};

    char* cur_word = malloc(20 * sizeof(char));
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
    return 0;
}
