#include <stdlib.h>
#include <stdatomic.h>
#include <pthread.h>
#include <assert.h>

#include "../signal_data_mismatch.c"

int main() {
    pthread_t p, c;

    if (pthread_create(&p, NULL, producer, NULL))
		abort();
	if (pthread_create(&c, NULL, consumer, NULL))
		abort();

    // pthread_create(&p, NULL, producer, NULL);
    // pthread_create(&c, NULL, consumer, NULL);
    // pthread_join(p, NULL);
    // pthread_join(c, NULL);
    return 0;
}