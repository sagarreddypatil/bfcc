#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>

void emitln(char *s, ...) {
    va_list args;
    va_start(args, s);
    printf("  ");
    vprintf(s, args);
    printf("\n");
    va_end(args);
}

void emitlnflat(char *s, ...) {
    va_list args;
    va_start(args, s);
    vprintf(s, args);
    printf("\n");
    va_end(args);
}

void emit(char *s, ...) {
    va_list args;
    va_start(args, s);
    vprintf(s, args);
    va_end(args);
}

typedef enum {
    NONE,
    PLUS,
    MINUS,
    NEXT,
    PREV,
    OUTC,
    INC,
    CONDSTART,
    CONDEND
} token_t;

token_t tokenize(char c) {
    switch (c) {
        case '+':
            return PLUS;
        case '-':
            return MINUS;
        case '>':
            return NEXT;
        case '<':
            return PREV;
        case '.':
            return OUTC;
        case ',':
            return INC;
        case '[':
            return CONDSTART;
        case ']':
            return CONDEND;
        default:
            return NONE;
    };
}

int loopstack[1024];
int loopstackptr = 0;
int loopidx = 0;

void enterloop() {
    loopstack[loopstackptr] = loopidx;
    loopstackptr++;
    loopidx++;
}

int getloopidx() {
    return loopstack[loopstackptr - 1];
}

void exitloop() {
    loopstackptr--;
}

void emitprologue() {
    emitlnflat(".text");
    emitlnflat(".global getchar, putchar, entrypoint\n");
    emitlnflat("entrypoint:");
    
    // first argument is the arena
    emitln("movq %%rdi, %%rbx");
}


int main(int argc, char *argv[]) {
    // get filename from first argument
    if (argc < 2) {
        printf("Usage: %s <filename>\n", argv[0]);
        return 1;
    }
    char *filename = argv[1];

    // open file
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error: Could not open file %s\n", filename);
        return 1;
    }

    emitprologue();

    token_t token = NONE;
    token_t prevtoken = NONE;

    char c;

    while ((c = fgetc(file)) != EOF) {
        token = tokenize(c);

        if (token == NONE) {
            continue;
        }

        switch(token) {
            case PLUS:
                emitln("addl $1, (%%rbx)");
                break;
            case MINUS:
                emitln("subl $1, (%%rbx)");
                break;
            case NEXT:
                emitln("addq $4, %%rbx");
                break;
            case PREV:
                emitln("subq $4, %%rbx");
                break;
            case OUTC:
                // call putchar
                emitln("movl (%%rbx), %%edi");
                emitln("call putchar");
                break;
            case INC:
                // call getchar
                emitln("call getchar");
                emitln("movl %%eax, (%%rbx)");
                break;
            case CONDSTART:
                enterloop();
                emitln("cmpl $0, (%%rbx)");
                emitln("je loopend_%d", getloopidx());
                emitlnflat("loopstart_%d:", getloopidx());
                break;
            case CONDEND:
                emitln("cmpl $0, (%%rbx)");
                emitln("jne loopstart_%d", getloopidx());
                emitlnflat("loopend_%d:", getloopidx());
                exitloop();
                break;

        };
        prevtoken = token;
    }

    emitln("movl $0, %%eax");
    emitln("ret");
}