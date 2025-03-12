#include <stdio.h>

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

    Node root = {};
    char c;
    while ((c = fgetc(words)) != EOF) {
    }

    if (!(feof(words))) { 
        // some other error? Handle?
    }

    fclose(words);
    return 0;
}
