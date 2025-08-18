#include <stdio.h>
#include <stdatomic.h>
#include <pthread.h>
#include <time.h>

atomic_int ready = 0;
atomic_int reordered_count = 0;

void delay_microseconds(int microseconds) {
    for (int i = 0; i < microseconds * 100; ++i) {
        __asm__ volatile ("" ::: "memory");
    }
}
void *writer_thread(void *arg) {
    delay_microseconds(100);
    atomic_store_explicit(&ready, 1, memory_order_relaxed);
    return NULL;
}

void *reader_thread(void *arg) {
    for (int i = 0; i < 100000; ++i) {
        int status = atomic_load_explicit(&ready, memory_order_relaxed);  
        if (status == 1) {
            delay_microseconds(1);  // 模拟 delay
            int check = atomic_load_explicit(&ready, memory_order_relaxed);  
            if (check != 1) {
                atomic_fetch_add(&reordered_count, 1);
            }
        }
    }
    return NULL;
}
