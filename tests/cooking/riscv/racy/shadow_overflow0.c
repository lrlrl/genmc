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

/* 递归函数触发栈溢出 */
__attribute__((noinline)) 
void stack_overflow(int depth, int thread_id) {
    char buffer[256]; // 占用栈空间（模拟栈增长）
    (void)buffer;     // 避免编译器优化
    
    if (depth >= MAX_DEPTH) {
        // -------------------------------
        // 修复前代码：直接访问影子堆栈（无锁保护）
        use_shadow_stack(thread_id);
        // -------------------------------
        return;
    }
    stack_overflow(depth + 1, thread_id); // 递归调用
}

/* 线程工作函数 */
void* thread_func(void* arg) {
    int thread_id = (int)(intptr_t)arg;
    stack_overflow(0, thread_id);
    return NULL;
}
