/* RISC-V 原子交换内存屏障问题示例（修复前） */
#include <stdatomic.h>
#include <stdbool.h>

typedef _Bool bool_t;

atomic_uintptr_t lock;       // 原子锁变量
int shared_data;             // 被保护的共享数据

/* 错误的原子交换函数：错误传递内存序参数 */
static inline bool_t try_arch_atomic_exchange(void *ptr, void *new_val, void **prev,
                                             int success_memorder, int failure_memorder) {
    // 错误：直接使用 success_memorder 参数（实际应处理失败路径）
    *prev = __atomic_exchange_n((void **)ptr, new_val, success_memorder);
    return (*prev == NULL); // 返回是否成功获取锁
}

/* 锁获取逻辑（错误应用内存序） */
void sel4_atomic_exchange(void *ptr, void *new_val, int memorder) {
    void *prev;
    do {
        // 错误：循环内使用强内存序（如 RELEASE）
        try_arch_atomic_exchange(ptr, new_val, &prev, memorder, memorder);
    } while (prev != NULL); // 自旋直到获取锁
    
    // 错误：未根据 memorder 添加手动屏障
}

/* 线程1：获取锁并修改数据 */
void *thread_1(void *arg) {
    // 错误：传递 ACQUIRE 内存序但未正确实现
    sel4_atomic_exchange(&lock, (void *)1, memory_order_acquire);
    
    // 临界区（无屏障保护）
    shared_data = 42;  // 可能重排到锁外
    
    // 错误：释放锁时使用 RELAXED
    atomic_store_explicit(&lock, 0, memory_order_relaxed);
    return NULL;
}

/* 线程2：读取数据 */
void *thread_2(void *arg) {
    // 错误：同上
    sel4_atomic_exchange(&lock, (void *)1, memory_order_acquire);
    
    // 可能读取到旧值
    int local_data = shared_data;
    
    atomic_store_explicit(&lock, 0, memory_order_relaxed);
    
    if (local_data != 42) {
        /* 数据竞争发生点 */
    }
    return NULL;
}
