#include <stdio.h>
#include <stdlib.h>

void entrypoint(char *arena);

int main() {
    char* arena = calloc(1024 * 1024, 1); // 1 MB
    entrypoint(arena);
    free(arena);
    return 0;
}