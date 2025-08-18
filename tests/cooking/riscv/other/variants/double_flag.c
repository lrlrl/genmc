#include <stdlib.h>
#include <stdatomic.h>
#include <pthread.h>
#include <assert.h>

#include "../double_flag.c"

int main() {
    pthread_t t1, t2, t3;
    // if(pthread_create(&t1, NULL, worker1, NULL))
    //     abort();
    // if(pthread_create(&t2, NULL, worker2, NULL))
    //     abort();
    // if(pthread_create(&t3, NULL, reader, NULL))
    //     abort();

    pthread_create(&t1, NULL, worker1, NULL);
    pthread_create(&t2, NULL, worker2, NULL);
    pthread_create(&t3, NULL, reader, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    pthread_join(t3, NULL);
    return 0;
}