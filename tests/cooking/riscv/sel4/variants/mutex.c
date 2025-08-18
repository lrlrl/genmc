#include <stdio.h>
#include <pthread.h>
#include <stdatomic.h>

// 引入主逻辑
#include "../mutex.c"

int main() {
    pthread_t t1, t2;


    pthread_create(&t1, NULL, thread_func, (void*)2);
    pthread_create(&t2, NULL, thread_func, (void*)1);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    return 0;
}
