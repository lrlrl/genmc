#include <stdlib.h>
#include <stdatomic.h>
#include <pthread.h>
#include <assert.h>

#include "../store_buffering.c"


int main() {
    pthread_t t1, t2;

    // if(pthread_create(&t1, NULL, thread1, NULL))
    //     abort();
    // if(pthread_create(&t2, NULL, thread2, NULL))
    //     abort();

    pthread_create(&t1, NULL, thread1, NULL);
    pthread_create(&t2, NULL, thread2, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    assert(r1 != 0 || r2 != 0);  // ❌ 在 RISC-V 上可能被击穿！
    return 0;
}