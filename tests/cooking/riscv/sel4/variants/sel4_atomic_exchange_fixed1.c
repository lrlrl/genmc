#include <stdlib.h>
#include <pthread.h>
#include <stdatomic.h>

#include "../sel4_atomic_exchange_fixed1.c"  // 包含主逻辑

int main() {
    pthread_t t1, t2;
    
  //  atomic_init(&lock, 0);  // 初始化锁
 //   shared_data = 0;        // 初始化共享数据
    
    pthread_create(&t1, NULL, thread_1, NULL);
    pthread_create(&t2, NULL, thread_2, NULL);
    
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    return 0;
}
