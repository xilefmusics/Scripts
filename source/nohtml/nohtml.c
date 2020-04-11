#include <stdio.h>
#include <string.h>
#include "../include/html_entity.h"

int main(int argc, char *argv[]) {
    char c;
    char buffer[10];
    int buffer_index;
    while ((c = getchar()) != EOF) {
        if (c == '<') {
            while ((c = getchar()) != EOF && c != '>') {}
        } else if (c == '&') {
            buffer_index = 0;
            do {
                buffer[buffer_index++] = c;
            } while(buffer_index < 8 && (c = getchar()) != EOF && c != ';');
            buffer[buffer_index++] = ';';
            buffer[buffer_index++] = '\0';
            for (int i = 0; i < 134; i++) {
                if (!strcmp(buffer, htmlentity_to_charstrings[i].htmlentity)) {
                    printf("%s", htmlentity_to_charstrings[i].charstring);
                    break;
                }
            }
        } else {
            putchar(c);
        }
    }
    return 0;
}
