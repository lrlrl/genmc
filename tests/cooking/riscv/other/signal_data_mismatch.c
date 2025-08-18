atomic_int signal = 0;
int data = 0;

void *producer(void *arg) {
    data = 100;
    atomic_store_explicit(&signal, 1, memory_order_release);
    return NULL;
}

void *consumer(void *arg) {
    while (atomic_load_explicit(&signal, memory_order_acquire) == 0);
    assert(data == 100);  // ❌ RISC-V 可能看到 data == 0
    return NULL;
}