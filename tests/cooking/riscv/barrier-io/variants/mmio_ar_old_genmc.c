#include <genmc.h>
#include <pthread.h>
#include <stdatomic.h>
#include <stdlib.h>
#include <assert.h>

atomic_int device_reg;
atomic_int ready;
pthread_barrier_t barrier;

void *device_writer(void *arg) {
	atomic_store_explicit(&device_reg, 0xAB, memory_order_relaxed);
	atomic_store_explicit(&ready, 1, memory_order_release);
	return NULL;
}

void *cpu_reader(void *arg) {
	pthread_barrier_wait(&barrier);

	if (atomic_load_explicit(&ready, memory_order_acquire)) {
		int val = atomic_load_explicit(&device_reg, memory_order_relaxed);

		// 模拟 delay（在未加 fence 的旧内核中可重排）
		int delay = 0;
		for (int i = 0; i < 1000; ++i) delay++;

		// GenMC 支持标准 assert
		assert(val == 0xAB);
	}
	return NULL;
}

int main() {
	pthread_t t1, t2;

	atomic_init(&device_reg, 0);
	atomic_init(&ready, 0);
	pthread_barrier_init(&barrier, NULL, 2);

	pthread_create(&t1, NULL, device_writer, NULL);
	pthread_create(&t2, NULL, cpu_reader, NULL);

	return 0;
}

