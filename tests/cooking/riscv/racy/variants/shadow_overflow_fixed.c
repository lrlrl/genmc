//rc11√ imm√ riscv√
#include <stdlib.h>
#include <pthread.h>
#include <stdatomic.h>

#include "../shadow_overflow_fixed.c" // 包含主逻辑

int main() {
    pthread_t t1, t2;
    //int id1 = 1, id2 = 2;
    
    atomic_init(&spin_lock, 0); // 初始化原子锁
    
    // 创建两个线程模拟双核竞态
    pthread_create(&t1, NULL, thread_func, (void*)1);
    pthread_create(&t2, NULL, thread_func, (void*)2);
    
    // 等待线程结束
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    
    return 0;
}
