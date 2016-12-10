#include <vector>
#include <utility> // for pair
#include <algorithm>
#include <iostream>
#include <cstdlib>
#include "../kxsort.h"
using namespace std;

typedef pair<uint64_t, uint64_t> P64;

struct RadixTraitsP64 {
    static const int nBytes = 16;
    int kth_byte(const P64 &x, int k) {
    	if (k >= 8) return x.first >> ((k - 8) * 8) & 0xFF;
    	return x.second >> (k * 8) & 0xFF;
    }
    bool compare(const P64 &x, const P64 &y) {
    	return x < y;
    }
};

vector<P64> gen(int N) {
	vector<P64> v(N);
	for (int i = 0; i < N; ++i) {
		v[i].first = ((uint64_t)rand() << 32) | rand();
		v[i].second = ((uint64_t)rand() << 32) | rand();
	}
	return v;
}

int main() {
	int N = 10000000;
	srand(time(NULL));
	vector<P64> v = gen(N);
	kx::radix_sort(v.begin(), v.end(), RadixTraitsP64());
	cout << is_sorted(v.begin(), v.end()) << endl;
	return 0;
}