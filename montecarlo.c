#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <time.h>

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("usage: %s <processes> <points>\n", argv[0]);
        return 1;
    }

    int p = atoi(argv[1]);
    long n = atol(argv[2]);

    long *inside = mmap(NULL, sizeof(long),
        PROT_READ | PROT_WRITE,
        MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    *inside = 0;
    long per_proc = n / p;

    for (int i = 0; i < p; i++) {
        if (fork() == 0) {
            unsigned int seed = time(NULL) ^ getpid();
            long local = 0;
            for (long j = 0; j < per_proc; j++) {
                double x = (double)rand_r(&seed) / RAND_MAX * 2 - 1;
                double y = (double)rand_r(&seed) / RAND_MAX * 2 - 1;
                if (x*x + y*y <= 1)
                    local++;
            }
            __sync_fetch_and_add(inside, local);
            exit(0);
        }
    }

    for (int i = 0; i < p; i++)
        wait(NULL);

    double pi = 4.0 * (*inside) / n;
    printf("estimated Pi = %.6f\n", pi);

    munmap(inside, sizeof(long));
    return 0;
}
