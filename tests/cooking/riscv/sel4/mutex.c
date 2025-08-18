#include <stdatomic.h>
#include <stdbool.h>
#include <stdint.h>

typedef _Bool bool_t;

atomic_uintptr_t lock;             // 原子锁变量
int shared_data = 0;               // 被保护的共享数据
atomic_int critical_section_flag;  // 标志临界区是否冲突
atomic_int violation_detected;     // 记录互斥失败

/* 错误的原子交换函数：直接传递 success_memorder 而忽略失败语义 */
static inline bool_t try_arch_atomic_exchange(void *ptr, void *new_val, void **prev,
                                              int success_memorder, int failure_memorder) {
    *prev = __atomic_exchange_n((void **)ptr, new_val, success_memorder);  // 错误使用
    return (*prev == NULL);  // 是否获得锁
}

/* 锁获取逻辑（错误使用内存序） */
void sel4_atomic_exchange(void *ptr, void *new_val, int memorder) {
    void *prev;
    do {
        try_arch_atomic_exchange(ptr, new_val, &prev, memorder, memorder);  // 错误：failure=success
    } while (prev != NULL);
    
    // 未添加 memory_order 手动屏障（错误）
}

/* 错误临界区线程函数 */
void *thread_func(void *arg) {
    int thread_id = (int)(intptr_t)arg;

    sel4_atomic_exchange(&lock, (void *)1, memory_order_acquire);  // 錯誤：acquire但未屏障

    // 模拟冲突检测：如果标志不是0，则说明另一个线程也进来了
    int expected = 0;
    if (!atomic_compare_exchange_strong(&critical_section_flag, &expected, thread_id)) {
        atomic_store(&violation_detected, 1);
    }

    // 非原子读改写操作：可能发生 lost update
    int temp = shared_data;
    shared_data = temp + 1;

    atomic_store(&critical_section_flag, 0);

    // 錯誤：锁释放使用 relaxed，可能导致其他线程先看到变量更新，再看到锁释放
    atomic_store_explicit(&lock, 0, memory_order_relaxed);
    return NULL;
}
