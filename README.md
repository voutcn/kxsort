## kxsort
Fast, in-place and STL container compatible radix sort.

### Quick Start

```cpp
#include "kxsort.h"

int main(int argc, char **argv) {
  int N = 10000000;
	vector<int> a(N);
	srand(10086);
	for (int i = 0; i < N; ++i) {
		a[i] = -rand() * (rand() % 2 == 1 ? 1 : -1);
	}

	kx::radix_sort(a.begin(), a.end());
	cout << (is_sorted(a.begin(), a.end()) ? "OK" : "Error") << endl;
    return 0;
}
```

### APIs

### Methodology

### Benchmarking
