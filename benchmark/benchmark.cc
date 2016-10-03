#include "../kxsort.h"
#include "ksort.h"
#include <cstdlib>
#include <cstdio>
#include <ctime>
#include <cstdint>
#include <algorithm>

using namespace std;

uint64_t rand64() {
	return ((uint64_t)rand() << 32) | rand();
}

#define REP 10

#define test_sort_i32(name, N, sort) \
do { \
	double tot_time = 0; \
	for (int re = 0; re < REP; ++re) { \
		int *a = new int[N]; \
		srand(10086); \
		for (size_t i = 0; i < N; ++i) { \
			a[i] = rand() * (rand() ? -1 : 1); \
		} \
		clock_t t1 = clock(); \
		sort; \
		clock_t t2 = clock(); \
		if (!is_sorted(a, a + N)) { \
			fprintf(stderr, "%s: incorrect!\n", name); \
		} \
		tot_time += (double)(t2 - t1) / CLOCKS_PER_SEC; \
		delete[] a; \
	} \
	fprintf(stderr, "%s int: %lf\n", name, tot_time / REP); \
} while (0);

#define test_sort_u32(name, N, sort) \
do { \
	double tot_time = 0; \
	for (int re = 0; re < REP; ++re) { \
		uint32_t *a = new uint32_t[N]; \
		srand(10086); \
		for (size_t i = 0; i < N; ++i) { \
			a[i] = rand(); \
		} \
		clock_t t1 = clock(); \
		sort; \
		clock_t t2 = clock(); \
		if (!is_sorted(a, a + N)) { \
			fprintf(stderr, "%s: incorrect!\n", name); \
		} \
		tot_time += (double)(t2 - t1) / CLOCKS_PER_SEC; \
		delete[] a; \
	} \
	fprintf(stderr, "%s uint32_t: %lf\n", name, tot_time / REP); \
} while (0);

#define test_sort_u64(name, N, sort) \
do { \
	double tot_time = 0; \
	for (int re = 0; re < REP; ++re) { \
		uint64_t *a = new uint64_t[N]; \
		srand(10086); \
		for (size_t i = 0; i < N; ++i) { \
			a[i] = rand64(); \
		} \
		clock_t t1 = clock(); \
		sort; \
		clock_t t2 = clock(); \
		if (!is_sorted(a, a + N)) { \
			fprintf(stderr, "%s: incorrect!\n", name); \
		} \
		tot_time += (double)(t2 - t1) / CLOCKS_PER_SEC; \
		delete[] a; \
	} \
	fprintf(stderr, "%s uint64_t: %lf\n", name, tot_time / REP); \
} while (0);

#define sort_key_u64(a) (a)
KRADIX_SORT_INIT(u64, uint64_t, sort_key_u64, 8)
#define sort_key_u32(a) (a)
KRADIX_SORT_INIT(u32, uint32_t, sort_key_u32, 4)
#define sort_key_i32(a) ((a) ^ 0x80000000)
KRADIX_SORT_INIT(i32, int, sort_key_i32, 4)

int main(int argc, char **argv) {
	unsigned long long n = 1000000000;
	if (argc < 2) {
		fprintf(stderr, "Usage: %s [n=%llu]\n", argv[0], n);
	} else {
		n = atoll(argv[1]);
	}

	for (unsigned long long N = 1000000; N <= n; N *= 10) {
		fprintf(stderr, "%llu:\n", N);
		test_sort_i32("kx::radix_sort", N, kx::radix_sort(a, a + N));
		test_sort_i32("klib_radix_sort", N, radix_sort_i32(a, a + N));
		test_sort_i32("std::sort", N, std::sort(a, a + N));

		test_sort_u32("kx::radix_sort", N, kx::radix_sort(a, a + N));
		test_sort_u32("klib_radix_sort", N, radix_sort_u32(a, a + N));
		test_sort_u32("std::sort", N, std::sort(a, a + N));

		test_sort_u64("kx::radix_sort", N, kx::radix_sort(a, a + N));
		test_sort_u64("klib_radix_sort", N, radix_sort_u64(a, a + N));
		test_sort_u64("std::sort", N, std::sort(a, a + N));
		fprintf(stderr, "\n");
	}

	return 0;
}