## kxsort
Fast, in-place radix sort.

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
To sort all C++ builtin integers in ascending order, the API is the same as *std::sort*.
```cpp
template <class RandomIt>
void radix_sort(RandomIt s, RandomIt e);
```

#### 2. Other class or struct

```cpp
template <class RandomIt, class RadixTraits>
inline void radix_sort(RandomIt s, RandomIt e, RadixTraits r_traits);
```

To sort items of other value types, the user should define the *RadixTraits* for them. *kx::radix_sort* sorts items byte by byte (i.e. the bucket size is 256 for each round). A *RadixTraits* struct must implement the following public interfaces,
```cpp
struct SampleRadixTraits {
    static const int nBytes;
    int kth_byte(const YourType &x, int k);
    bool compare(const YourType &x, const YourType &y);
};
```
where `nBytes` specifies how many rounds the radix sort will go over, and `kth_byte(const YourType &x, int k)`  is the function to extract the k-th (0-base) byte of `x` for radix sort, i.e. the byte to be used in the `nBytes-1-k` round.

The function `compare` should be compatible with `kth_byte`, which means it should return exactly the same results as the following function for any `x` and `y`, but the implementation may be faster:

```cpp
bool another_compare(const YourType &x, const YourType &y) {
	for (int k = nBytes - 1; k >= 0; --k) {
		if (kth_byte(x, k) < kth_byte(y, k)) return true;
		if (kth_byte(x, k) > kth_byte(y, k)) return false;
	}
	return false;
}
```

You may want to look at some [examples](https://github.com/voutcn/kxsort/tree/master/examples).

### Methodology

An [MSD radix sort](https://en.wikipedia.org/wiki/Radix_sort#Most_significant_digit_radix_sorts) is implemented in *kx::radix_sort*. It partitions the input array with the most significant byte, and sort each partitions recursively. When the number of items in a partition is no more than 64, an insertion sort will be applied and no more recursion is needed. The using of insertion sort is the reason why it requires a `compare` function in *RadixTraits*.

The implement here is substantially the same as the following implementations
- [ac's klib radix_sort](https://attractivechaos.wordpress.com/2012/06/10/an-update-on-radix-sort/)
- [Gorset's implementation](https://github.com/gorset/radix)
- [Birkeland's PhD thesis](http://ntnu.diva-portal.org/smash/record.jsf?parentRecord=diva2%3A124514&pid=diva2%3A124519&dswid=-3545)

*kx::radix_sort* is a bit faster than ac's *klib radix_sort* probably due to the using of C++ template, which may unfold the recursions.

### Limitations

* It is not a stable sorting method.
* It is slower than *std::sort* if the input sequence is nearly sorted.
