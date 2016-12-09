## kxsort
Fast, in-place and STL-like radix sort.

### Quick Start

```cpp
#include "kxsort.h"
#include <cstdlib>
#include <vector>
#include <iostream>

int main(int argc, char **argv) {
	int N = 10000000;
	std::vector<int> a(N);
	for (int i = 0; i < N; ++i) {
		a[i] = rand() * (rand() % 2 ? 1 : -1);
	}

	kx::radix_sort(a.begin(), a.end());
	std::cout << (is_sorted(a.begin(), a.end()) ? "OK" : "Error") << std::endl;
	return 0;
}
```

### Benchmarks

Time (sec.) for sorting 100 million random integers.

|Implementation  | int      | uint32_t |  uint64_t |
|:---------------|---------:|---------:|----------:|
|kx::radix_sort  | 4.630    | 4.621    | 5.304     |
|klib radix_sort | 5.074    | 5.233    | 5.926     |
|std::sort       | 11.321   | 11.191   | 11.260    |


### APIs
#### 1. Builtin Integers
For sort all C++ builtin integers in ascending order, the API is the same as std::sort.
```cpp
template <class RandomIt>
void radix_sort(RandomIt s, RandomIt e)
```

#### 2. User-defined structures
For sorting user-defined structure, the user should also define a "RadixByte" struct and a "Compare" function (or functional objects).

```cpp
template <class RandomIt, class RadixTrait>
inline void radix_sort(RandomIt s, RandomIt e, RadixTrait r_trait)
```

`kx::radix_sort` will sort items byte by byte (i.e. the bucket size is 256 for each round). A `RadixTrait` struct must implements the following public interfaces,
```cpp
struct SampleRadixTrait {
    static const int nBytes;
    int kth_byte(const T &x, int k);
    bool compare(const T &x, const T &y);
};
```
where `nBytes` specifies how many passes the radix sort will go over, and `kth_byte(const T &x, int k)`  is the function to extract the k-th byte of the `T` for radix sort, i.e. the byte used to process the `nBytes - 1 - k` round.

The function `compare(const T &x, const T &y)` should be compatible with `kth_byte`, which means it should return the same value as the following function:

```cpp
bool another_compare(const T &x, const T &y) {
	for (int k = nBytes - 1; k >= 0; --k) {
		if (kth_byte(x) < kth_byte(y)) return true;
		if (kth_byte(x) > kth_byte(y)) return false;
	}
	return false;
}
```

You may want to look at some [examples](https://github.com/voutcn/kxsort/tree/examples).

### Methodologies

To be filled...

### Limitations

* It is not a stable sorting method.
* It is slower than `std::sort` if the input sequence is nearly sorted.
