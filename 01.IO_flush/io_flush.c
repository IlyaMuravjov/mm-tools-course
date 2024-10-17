#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    int opt;
    int add_newline = 0;
    int remove_fflush = 0;

    while ((opt = getopt(argc, argv, "nf")) != -1) {
        switch (opt) {
            case 'n':
                add_newline = 1;
                break;
            case 'f':
                remove_fflush = 1;
                break;
            default:
                fprintf(stderr, "Usage: %s [-n] [-f]\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    if (add_newline) {
        puts("STDOUT\n");
    } else {
        puts("STDOUT");
    }

    if (!remove_fflush) {
        fflush(stdout);
    }

    if (add_newline) {
        fputs("STDERR\n", stderr);
    } else {
        fputs("STDERR", stderr);
    }

    if (!remove_fflush) {
        fflush(stderr);
    }

    return 0;
}

