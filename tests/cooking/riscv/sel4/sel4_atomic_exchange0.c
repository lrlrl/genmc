/* RISC-V 原子交换内存屏障问题示例（修复前） */
#include <stdatomic.h>
#include <stdbool.h>

atomic_int lock = ATOMIC_VAR_INIT(0);  // 原子锁变量
int shared_data = 0;                   // 被保护的共享数据

/* 错误的原子交换函数：使用强内存序 */
bool try_arch_atomic_exchange(atomic_int *ptr, int new_val, int *prev_val) {
    // 错误：使用 memory_order_acquire（过度同步）
    *prev_val = atomic_exchange_explicit(ptr, new_val, memory_order_acquire);
    return (*prev_val == 0);
}

/* 线程1：错误的内存序控制 */
void *thread_1(void *arg) {
    int prev;
    do {
        try_arch_atomic_exchange(&lock, 1, &prev); // Acquire内存序增加延迟
    } while (prev != 0);
    
    // 无手动屏障，临界区操作可能重排到锁外
    shared_data = 42; 
    
    // 错误的锁释放：仅用 RELAXED 存储
    atomic_store_explicit(&lock, 0, memory_order_relaxed);
    return NULL;
}

/* 线程2：可能读取到旧数据 */
void *thread_2(void *arg) {
    int prev;
    do {
        try_arch_atomic_exchange(&lock, 1, &prev); // 同样的过度同步
    } while (prev != 0);
    
    // 无屏障，可能读取到未更新的 shared_data
    int local_data = shared_data; 
    
    atomic_store_explicit(&lock, 0, memory_order_relaxed);
    
    if (local_data != 42) {
        /* 数据竞争发生点 */
    }
    return NULL;
}
