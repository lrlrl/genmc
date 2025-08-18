//rc11× imm√ riscv×
#include <stdlib.h>
#include <pthread.h>
#include <stdatomic.h>

#include "../shadow_overflow0.c" // 包含主逻辑

int main() {
    pthread_t t1, t2;

    // // 创建两个线程模拟双核竞态
    // if (pthread_create(&t1, NULL, thread_func, &id1)) 
    //     abort();
    // if (pthread_create(&t2, NULL, thread_func, &id2)) 
    //     abort();
    
    // 创建两个线程模拟双核竞态
    pthread_create(&t1, NULL, thread_func, (void*)1);
    pthread_create(&t2, NULL, thread_func, (void*)2);
    // 等待线程结束
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    
    return 0;
}
