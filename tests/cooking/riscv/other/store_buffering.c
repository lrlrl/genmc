atomic_int x = 0, y = 0;
int r1 = 0, r2 = 0;

void *thread1(void *arg) {
    atomic_store_explicit(&x, 1, memory_order_relaxed);
    r1 = atomic_load_explicit(&y, memory_order_relaxed);
    return NULL;
}

void *thread2(void *arg) {
    atomic_store_explicit(&y, 1, memory_order_relaxed);
    r2 = atomic_load_explicit(&x, memory_order_relaxed);
    return NULL;
}