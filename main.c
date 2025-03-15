#include <stdio.h>
#include <stdlib.h>

#include "trie.h"
typedef struct Node {
    struct Node* children;
    size_t count;
    char letter;
} Node;

int main() {
    FILE* words = fopen("/usr/share/dict/words", "r");
    // 479826 lines
    if (!words) {
        perror("ERROR: Failed to open words file");
        return -1;
    }

    Node root = {0};
    Node* children = malloc(1024 * sizeof(Node));

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
            cur_word[word_len] = c;
            word_len++;

            // traverse the tree
            Node* cur_node = &root;
            // while c not in cur_node->children
            while (cur_node->children
           
        }
    }

    if (!(feof(words))) { 
        // some other error? Handle?
    }
    
    free(cur_word);
    free(children);
    fclose(words);
    return 0;
}
