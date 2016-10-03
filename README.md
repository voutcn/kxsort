## kxsort
Fast, in-place and STL container compatible radix sort.

### Quick Start

```cpp
#include "kxsort.h"
#include <cstdlib>
#include <vector>
#include <iostream>

int main(int argc, char **argv) {
	int N = 10000000;
	std::vector<int> a(N);
	srand(10086);
	for (int i = 0; i < N; ++i) {
		a[i] = rand() * (rand() % 2 ? 1 : -1);
	}

	kx::radix_sort(a.begin(), a.end());
	std::cout << (is_sorted(a.begin(), a.end()) ? "OK" : "Error") << std::endl;
	return 0;
}
```

### Benchmarking

Time (sec.) for sorting 100 million integers.

|Implementation  | int      | uint32_t |  uint64_t |
|:---------------|---------:|---------:|----------:|
|kx::radix_sort  | 4.630    | 4.621    | 5.304     |
|klib_radix_sort | 5.074    | 5.233    | 5.926     |
|std::sort       | 11.321   | 11.191   | 11.260    |


### APIs

### Methodology
