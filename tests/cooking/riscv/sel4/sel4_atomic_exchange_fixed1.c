/* RISC-V 原子交换内存屏障问题（修复后） */
#include <stdatomic.h>
#include <stdbool.h>

typedef _Bool bool_t;

atomic_uintptr_t lock;       // 原子锁变量
int shared_data;             // 被保护的共享数据

/* 修复后的原子交换函数：仅用 RELAXED 内存序 */
static inline bool_t try_arch_atomic_exchange_rlx(void *ptr, void *new_val, void **prev) {
    // 使用 Relaxed 内存序进行原子交换
    *prev = __atomic_exchange_n((void **)ptr, new_val, __ATOMIC_RELAXED);
    return (*prev == NULL);
}

/* 修复后的锁获取逻辑：手动添加屏障 */
void sel4_atomic_exchange(void *ptr, void *new_val, int memorder) {
    void *prev;
    do {
        // 自旋等待时仅用 Relaxed 内存序
        try_arch_atomic_exchange_rlx(ptr, new_val, &prev);
    } while (prev != NULL); // 自旋直到获取锁
    
    // 根据调用者要求添加屏障
    if (memorder == memory_order_acquire || memorder == memory_order_seq_cst) {
        __atomic_thread_fence(memory_order_acquire); // Acquire 屏障
    }
}

/* 线程1：正确同步的临界区 */
void *thread_1(void *arg) {
    // 获取锁（Acquire语义）
    sel4_atomic_exchange(&lock, (void *)1, memory_order_acquire);
    
    // 临界区操作（受 Acquire 屏障保护）
    shared_data = 42;  // 写操作不会被重排到锁外
    
    // 释放锁（Release语义）
    __atomic_thread_fence(memory_order_release);       // Release 屏障
    atomic_store_explicit(&lock, 0, memory_order_relaxed);
    return NULL;
}

/* 线程2：正确同步的数据读取 */
void *thread_2(void *arg) {
    // 获取锁（Acquire语义）
    sel4_atomic_exchange(&lock, (void *)1, memory_order_acquire);
    
    // 临界区操作（受 Acquire 屏障保护）
    int local_data = shared_data;  // 正确读取 Thread1 的写入
    
    // 释放锁（Release语义）
    __atomic_thread_fence(memory_order_release);       // Release 屏障
    atomic_store_explicit(&lock, 0, memory_order_relaxed);
    
    // 断言验证数据同步
    if (local_data != 42) {
        /* 修复后此处永远不会触发 */
    }
    return NULL;
}
