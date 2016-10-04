/* The MIT License
   Copyright (c) 2016 Dinghua Li <voutcn@gmail.com>
   
   Permission is hereby granted, free of charge, to any person obtaining
   a copy of this software and associated documentation files (the
   "Software"), to deal in the Software without restriction, including
   without limitation the rights to use, copy, modify, merge, publish,
   distribute, sublicense, and/or sell copies of the Software, and to
   permit persons to whom the Software is furnished to do so, subject to
   the following conditions:
   
   The above copyright notice and this permission notice shall be
   included in all copies or substantial portions of the Software.
   
   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
   NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
   BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
   ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
   CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
   SOFTWARE.
*/

#ifndef KXSORT_H__
#define KXSORT_H__

#include <stdint.h>
#include <memory.h>
#include <iterator>
#include <algorithm>

namespace kx {

static const int kRadixBits = 8;
static const size_t kInsertSortThreshold = 64;
static const int kRadixMask = (1 << kRadixBits) - 1;
static const int kRadixBin = 1 << kRadixBits;

//================= HELPING FUNCTIONS ====================

template <class T>
struct GetKeyUnsignedInteger {
    static const int nBits = sizeof(T) * 8;
    T operator() (const T &x) {
        return x;
    }
};

template <class T>
struct GetKeySignedInteger {
    static const T kMSB = T(0x80) << ((sizeof(T) - 1) * 8);
    static const int nBits = sizeof(T) * 8;
    T operator() (const T &x) {
        return x ^ kMSB;
    }
};

template <class GetKey, class T>
struct CompareKey {
    bool operator() (const T &a, const T &b) {
        return GetKey()(a) < GetKey()(b);
    }
};

template <int N>
struct GenericUint {
    static const int kNumUint64 = (N + 7) / 8;
    uint64_t data_[kNumUint64];

    GenericUint() {
        memset(data_, 0, sizeof(data_));
    }

    void set(int pos, int length, uint64_t val) {
        int offset = pos % 64;
        int idx = pos / 64;
        data_[idx] |= val << offset;
        if (offset + length > 64) {
            data_[idx+1] |= val >> (64 - offset);
        }
    }

    bool operator<(const GenericUint<N> &rhs) const {
        for (int i = kNumUint64 - 1; i >= 0; --i) {
            if (data_[i] != rhs.data_[i]) return data_[i] < rhs.data_[i];
        }
        return false;
    }
};

template <int pos, int length, int N>
void set(GenericUint<N> &a, uint64_t val) {
    const int offset = pos % 64;
    const int idx = pos / 64;
    a.data_[idx] |= val << offset;
    if (offset + length > 64) {
        a.data_[idx+1] |= val >> (64 - offset);
    }
}

template <class T>
unsigned ShiftAndMask(const T &x, int k) {
    return (x >> k) & kRadixMask;
}

template <int N>
unsigned ShiftAndMask(const GenericUint<N> &x, int k) {
    return (x.data_[k / 64] >> k % 64) & kRadixMask;
}

template <class RandomIt, class ValueType, class Compare>
inline void insert_sort_core_(RandomIt s, RandomIt e, Compare cmp)
{
    for (RandomIt i = s + 1; i < e; ++i) {
        if (cmp(*i, *(i - 1))) {
            RandomIt j;
            ValueType tmp = *i;
            *i = *(i - 1);
            for (j = i - 1; j > s && cmp(tmp, *(j - 1)); --j) {
                *j = *(j - 1);
            }
            *j = tmp;
        }
    }
}

template <class RandomIt, class ValueType, class GetKey, class Compare, int kShift>
inline void radix_sort_core_(RandomIt s, RandomIt e, GetKey get_key, Compare cmp)
{
    RandomIt last_[kRadixBin + 1];
    RandomIt *last = last_ + 1;
    size_t count[kRadixBin] = {0};

    for (RandomIt i = s; i < e; ++i) {
        ++count[ShiftAndMask(get_key(*i), kShift)];
    }

    last_[0] = last_[1] = s;
    for (int i = 1; i < kRadixBin; ++i) {
        last[i] = last[i-1] + count[i-1];
    }

    for (int i = 0; i < kRadixBin; ++i) {
        RandomIt end = last[i-1] + count[i];
        while (last[i] != end) {
            ValueType swapper = *last[i];
            int tag = ShiftAndMask(get_key(swapper), kShift);
            if (tag != i) {
                do {
                    std::swap(swapper, *last[tag]++);
                } while ((tag = ShiftAndMask(get_key(swapper), kShift)) != i);
                *last[i] = swapper;
            }
            ++last[i];
        }
    }

    if (kShift > 0) {
        for (int i = 0; i < kRadixBin; ++i) {
            if (count[i] > kInsertSortThreshold) {
                radix_sort_core_<RandomIt, ValueType, GetKey, Compare, 
                                  (kShift > 0 ? (kShift - kRadixBits) : 0)>
                                  (last[i-1], last[i], get_key, cmp);
            } else if (count[i] > 1) {
                insert_sort_core_<RandomIt, ValueType, Compare>(last[i-1], last[i], cmp);
            }
        }
    }
}

template <class RandomIt, class ValueType, class GetKey, class Compare>
inline void radix_sort_entry_(RandomIt s, RandomIt e, GetKey get_key, Compare cmp)
{
    if (e - s <= (int)kInsertSortThreshold)
        insert_sort_core_<RandomIt, ValueType, Compare>(s, e, cmp);
    else if (GetKey::nBits % kRadixBits == 0)
        radix_sort_core_<RandomIt, ValueType, GetKey, Compare, GetKey::nBits - kRadixBits>(s, e, get_key, cmp);
    else
        radix_sort_core_<RandomIt, ValueType, GetKey, Compare, GetKey::nBits - GetKey::nBits % kRadixBits>(s, e, get_key, cmp);
}

template <class RandomIt, class ValueType, class GetKey>
inline void radix_sort_entry_(RandomIt s, RandomIt e, ValueType*, GetKey get_key)
{
    radix_sort_entry_<RandomIt, ValueType, GetKey, CompareKey<GetKey, ValueType> >(s, e, get_key, CompareKey<GetKey, ValueType>());
}

template <class RandomIt, class ValueType>
inline void radix_sort_entry_(RandomIt s, RandomIt e, ValueType *)
{
    if (ValueType(-1) > ValueType(0)) {
        radix_sort_entry_<RandomIt, ValueType, GetKeyUnsignedInteger<ValueType>, std::less<ValueType> >(s, e, GetKeyUnsignedInteger<ValueType>(), std::less<ValueType>());
    } else {
        radix_sort_entry_<RandomIt, ValueType, GetKeySignedInteger<ValueType>, std::less<ValueType> >(s, e, GetKeySignedInteger<ValueType>(), std::less<ValueType>());
    }
}

//================= INTERFACES ====================

template <class RandomIt, class GetKey>
inline void radix_sort(RandomIt s, RandomIt e, GetKey get_key)
{
    typename std::iterator_traits<RandomIt>::value_type *dummy(0);
    radix_sort_entry_(s, e, dummy, get_key);
}

template <class RandomIt>
inline void radix_sort(RandomIt s, RandomIt e)
{
    typename std::iterator_traits<RandomIt>::value_type *dummy(0);
    radix_sort_entry_(s, e, dummy);
}

}

#endif