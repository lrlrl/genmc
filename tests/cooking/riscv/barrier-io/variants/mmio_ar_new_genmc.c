#include <stdatomic.h>
#include <pthread.h>
#include <stdio.h>

atomic_int reg_status;   // 模拟设备状态寄存器 (I/O)
atomic_int reg_trigger;  // 模拟控制寄存器触发操作

/* 修复前的I/O读实现（不完整内存屏障） */
int read_status_faulty() {
    int val = atomic_load_explicit(&reg_status, memory_order_relaxed);
    // 模拟原补丁修复前的屏障 fence i,r -> 不约束后续I/O读
    atomic_thread_fence(memory_order_acquire); 
    return val;
}

/* 子线程：模拟设备异步处理 */
void *device_thread(void *unused) {
    // 等待主线程触发操作
    while (atomic_load_explicit(&reg_trigger, memory_order_relaxed) == 0) {
        // 空转等待
    }
    
    // 模拟设备完成工作后更新状态
    atomic_store_explicit(&reg_status, 1, memory_order_relaxed);
    return NULL;
}

int main() {
    pthread_t t;
    
    // 初始状态：未触发、未完成
    atomic_init(&reg_status, 0);
    atomic_init(&reg_trigger, 0);
    
    pthread_create(&t, NULL, device_thread, NULL);
    
    // 步骤1：触发设备操作（写控制寄存器）
    atomic_store_explicit(&reg_trigger, 1, memory_order_relaxed);
    
    // 步骤2：轮询状态（模拟存在delay循环）
    int status;
    do {
        status = read_status_faulty();  // 可能乱序的I/O读
    } while (status == 0);              // 循环等待
    
    printf("Device operation completed!\n");
    
    pthread_join(t, NULL);
    return 0;
}
