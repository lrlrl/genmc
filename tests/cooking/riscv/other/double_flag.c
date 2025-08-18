atomic_int ready1 = 0;
atomic_int ready2 = 0;

void *worker1(void *arg) {
    atomic_store_explicit(&ready1, 1, memory_order_release);
    return NULL;
}

void *worker2(void *arg) {
    atomic_store_explicit(&ready2, 1, memory_order_release);
    return NULL;
}

void *reader(void *arg) {
    while (!(atomic_load_explicit(&ready1, memory_order_acquire) &&
             atomic_load_explicit(&ready2, memory_order_acquire)))
        ;

    // 临界区：假设两个标志置位后做某事
    return NULL;
}