// riscv/variants/read_delay.c
#include <pthread.h>
#include <stdio.h>
#include <stdatomic.h>

#include "../read_delay.c"  // 引入主逻辑

int main() {
    pthread_t t1, t2;

    atomic_store_explicit(&ready, 0, memory_order_relaxed);

    pthread_create(&t1, NULL, writer_thread, NULL);
    pthread_create(&t2, NULL, reader_thread, NULL);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    printf("检测到的读-延迟乱序次数：%d\n", reordered_count);
    return 0;
}
