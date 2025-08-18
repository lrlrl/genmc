/* riscv_atomic_barrier_fixed.c */
#include <stdatomic.h>
#include <stdbool.h>

atomic_int lock = ATOMIC_VAR_INIT(0);  // 使用标准原子类型
int shared_data = 0;

bool try_arch_atomic_exchange_rlx(atomic_int *ptr, int new_val, int *prev_val) {
    *prev_val = atomic_exchange_explicit(ptr, new_val, memory_order_relaxed);
    return (*prev_val == 0);
}

void *thread_1(void *arg) {
    int prev;
    do {
        try_arch_atomic_exchange_rlx(&lock, 1, &prev);
    } while (prev != 0);
    
    // Acquire屏障
    atomic_thread_fence(memory_order_acquire);
    
    shared_data = 42;
    
    // Release屏障
    atomic_thread_fence(memory_order_release);
    atomic_store_explicit(&lock, 0, memory_order_relaxed);
    return NULL;
}

void *thread_2(void *arg) {
    int prev;
    do {
        try_arch_atomic_exchange_rlx(&lock, 1, &prev);
    } while (prev != 0);
    
    atomic_thread_fence(memory_order_acquire);
    int local_data = shared_data;
    
    atomic_thread_fence(memory_order_release);
    atomic_store_explicit(&lock, 0, memory_order_relaxed);
    
    if (local_data != 42) {
        /* 数据竞争检测点 */
    }
    return NULL;
}
