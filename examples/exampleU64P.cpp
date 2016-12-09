#include <vector>
#include <utility> // for pair
#include <algorithm>
#include <iostream>
#include <cstdlib>
#include "../kxsort.h"
using namespace std;

struct RadixTraitU64P {
    static const int nBytes = 16;
    int kth_byte(const pair<uint64_t, uint64_t> &x, int k) {
    	if (k >= 8) return x.first >> ((k - 8) * 8) & 0xFF;
    	return x.second >> (k * 8) & 0xFF;
    }
    bool compare(const pair<uint64_t, uint64_t> &x, const pair<uint64_t, uint64_t> &y) {
    	return x < y;
    }
};

vector<pair<uint64_t, uint64_t> > gen(int N) {
	vector<pair<uint64_t, uint64_t> > v(N);
	for (int i = 0; i < N; ++i) {
		v[i].first = ((uint64_t)rand() << 32) | rand();
		v[i].second = ((uint64_t)rand() << 32) | rand();
	}
	return v;
}

int main() {
	int N = 10000000;
	srand(time(NULL));
	vector<pair<uint64_t, uint64_t> > v = gen(N);
	kx::radix_sort(v.begin(), v.end(), RadixTraitU64P());
	cout << is_sorted(v.begin(), v.end()) << endl;
	return 0;
}