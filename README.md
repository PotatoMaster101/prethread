# Pre-Thread
Simple POSIX threads pre-threading in C99. Originally used in uni assignments.

## Compile
Compile with `-lpthread` flag.
```
gcc *.c -lpthread -std=c99
```

## Example
```c
#include <stdio.h>
#include <unistd.h>
#include "prethd.h"

prethd_t *pool = NULL;

void *print(void *arg) {
    prethd_lock(pool, 0);       // locks first (and only) mutex
    fputc('H', stdout);
    fputc('e', stdout);
    fputc('l', stdout);
    fputc('l', stdout);
    fputc('o', stdout);
    fputc(' ', stdout);
    fputc('W', stdout);
    fputc('o', stdout);
    fputc('r', stdout);
    fputc('l', stdout);
    fputc('d', stdout);
    fputc('!', stdout);
    fputc('\n', stdout);
    prethd_unlock(pool, 0);     // unlocks first (and only) mutex
    sleep(5);
    return NULL;
}

int main(void) {
    const size_t NUM_TH = 50;       // number of threads
    const size_t NUM_MUT = 1;       // number of mutexes
    const size_t NUM_COND = 0;      // number of conditional variables

    pool = prethd_new(NUM_TH, NUM_MUT, NUM_COND);
    prethd_all(pool, print, NULL);
    prethd_join_free(pool);
    printf("Done\n");
    return 0;
}
```
