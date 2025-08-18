#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdatomic.h>
#include <genmc.h>

atomic_int normal_mem;
volatile int io_mem;  // 模拟 MMIO 地址

pthread_barrier_t barrier;

void __io_bw() {
    __asm__ __volatile__("fence w,o" ::: "memory"); // 模拟 fence for I/O write
}

void __io_ar() {
    __asm__ __volatile__("fence i,ir" ::: "memory"); // 模拟 fence for I/O read
}

void *thread_writer(void *unused) {
    pthread_barrier_wait(&barrier);

    // Step 1: write to normal memory
    atomic_store_explicit(&normal_mem, 1, memory_order_relaxed);

    // Step 2: memory barrier before writing to IO
    __io_bw();

    // Step 3: write to MMIO
    io_mem = 1;

    return NULL;
}

void *thread_reader(void *unused) {
    pthread_barrier_wait(&barrier);

    int observed_io = io_mem;
    __io_ar(); // memory barrier after IO read
    int observed_normal = atomic_load_explicit(&normal_mem, memory_order_relaxed);

    // 验证：若看到 IO == 1，则 normal 也应为 1
    if (observed_io == 1 && observed_normal == 0) {
        printf("Reordering detected: IO=1 but Normal=0\n");
        GENMC_ASSERT(0);  // 模型检查时触发错误
    }

    return NULL;
}

