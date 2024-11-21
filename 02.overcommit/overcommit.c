#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define PAGE_SIZE (4 * 1024)
#define SLEEP_EVERY_PAGES 100000
#define SLEEP_MILLIS 100

int main(int argc, char const *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <array size in GB>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    
    char *endptr;
    long size_in_gb = strtol(argv[1], &endptr, 10);

    if (*endptr != '\0' || endptr == argv[1] || size_in_gb <= 0) {
        fprintf(stderr, "Invalid array size: '%s'\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    size_t size_in_bytes = (size_t) size_in_gb * 1024 * 1024 * 1024;

    char *array = malloc(size_in_bytes);
    if (array == NULL) {
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }

    size_t page_count = size_in_bytes / PAGE_SIZE;

    for (size_t page = 0; page < page_count; page++) {
        if (page % SLEEP_EVERY_PAGES == 0) {
            printf("Page %zuk out of %zuk\n", page / 1000, page_count / 1000);
            
            struct timespec sleep_time;
            sleep_time.tv_sec = 0;
            sleep_time.tv_nsec = SLEEP_MILLIS * 1000000;
            nanosleep(&sleep_time, NULL);
        }
        array[page * PAGE_SIZE] = 0;
    }

    free(array);

    return 0;
}
