#include <stdio.h>
#include <pthread.h>
#include <stdatomic.h>

#define MAX_DEPTH 10       // 触发模拟栈溢出的递归深度
#define SHADOW_STACK_SIZE 1024

/* 共享的"影子堆栈"区域（修复前无保护） */
char shadow_stack[SHADOW_STACK_SIZE];
atomic_int spin_lock;       // 原子锁（修复前未使用）

/* 模拟使用影子堆栈（修复前会并发访问） */
void use_shadow_stack(int thread_id) {
    static int shared_counter = 0; // 模拟共享状态
    
    // 操作影子堆栈（此处简化为修改共享变量）
    shared_counter++;
    
}

/* 递归函数触发栈溢出（修复后） */
__attribute__((noinline)) 
void stack_overflow(int depth, int thread_id) {
    char buffer[256];
    (void)buffer;
    
    if (depth >= MAX_DEPTH) {
        // -------------------------------
        // 修复后代码：通过原子锁保护影子堆栈
        int expected = 0;
        while (!atomic_compare_exchange_strong(&spin_lock, &expected, 1)) {
            expected = 0; // 自旋等待锁释放
        }
        use_shadow_stack(thread_id);
        atomic_store(&spin_lock, 0); // 释放锁
        // -------------------------------
        return;
    }
    stack_overflow(depth + 1, thread_id);
}
/* 线程工作函数 */
void* thread_func(void* arg) {
    int thread_id = (int)(intptr_t)arg;
    stack_overflow(0, thread_id);
    return NULL;
}
