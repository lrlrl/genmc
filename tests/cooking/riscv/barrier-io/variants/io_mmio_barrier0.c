#include <pthread.h>
#include <stdlib.h>

#include "../io_mmio_barrier.c"

int main() {
    pthread_t t1, t2;

    pthread_barrier_init(&barrier, NULL, 2);

    if (pthread_create(&t1, NULL, thread_writer, NULL)) abort();
    if (pthread_create(&t2, NULL, thread_reader, NULL)) abort();

    return 0;
}

