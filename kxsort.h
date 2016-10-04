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

#include <iterator>
#include <algorithm>

namespace kx {

static const int kRadixBits = 8;
static const size_t kInsertSortThreshold = 64;
static const int kRadixMask = (1 << kRadixBits) - 1;
static const int kRadixBin = 1 << kRadixBits;

//================= HELPING FUNCTIONS ====================

template <class T>
struct RadixByteUnsigned {
    static const int nBytes = sizeof(T);
    int operator() (const T &x, int k) { return (x >> (kRadixBits * k)) & kRadixMask; }
};

template<class T>
struct RadixByteSigned {
    static const int nBytes = sizeof(T);
    static const T kMSB = T(0x80) << ((sizeof(T) - 1) * 8);
    int operator() (const T &x, int k) {
        return ((x ^ kMSB) >> (kRadixBits * k)) & kRadixMask;
    }
};

template <class RandomAccessIterator, class ValueType, class Compare>
inline void insert_sort_core_(RandomAccessIterator s, RandomAccessIterator e, Compare cmp)
{
    for (RandomAccessIterator i = s + 1; i < e; ++i) {
        if (cmp(*i, *(i - 1))) {
            RandomAccessIterator j;
            ValueType tmp = *i;
            *i = *(i - 1);
            for (j = i - 1; j > s && cmp(tmp, *(j - 1)); --j) {
                *j = *(j - 1);
            }
            *j = tmp;
        }
    }
}

template <class RandomAccessIterator, class ValueType, class RadixByte, class Compare, int kWhichByte>
inline void radix_sort_core_(RandomAccessIterator s, RandomAccessIterator e, RadixByte rbyte, Compare cmp)
{
    RandomAccessIterator last_[kRadixBin + 1];
    RandomAccessIterator *last = last_ + 1;
    size_t count[kRadixBin] = {0};

    for (RandomAccessIterator i = s; i < e; ++i) {
        ++count[rbyte(*i, kWhichByte)];
    }

    last_[0] = last_[1] = s;
    for (int i = 1; i < kRadixBin; ++i) {
        last[i] = last[i-1] + count[i-1];
    }

    for (int i = 0; i < kRadixBin; ++i) {
        RandomAccessIterator end = last[i-1] + count[i];
        while (last[i] != end) {
            ValueType swapper = *last[i];
            int tag = rbyte(swapper, kWhichByte);
            if (tag != i) {
                do {
                    std::swap(swapper, *last[tag]++);
                } while ((tag = rbyte(swapper, kWhichByte)) != i);
                *last[i] = swapper;
            }
            ++last[i];
        }
    }

    if (kWhichByte > 0) {
        for (int i = 0; i < kRadixBin; ++i) {
            if (count[i] > kInsertSortThreshold) {
                radix_sort_core_<RandomAccessIterator, ValueType, RadixByte, Compare,
                                  (kWhichByte > 0 ? (kWhichByte - 1) : 0)>
                                  (last[i-1], last[i], rbyte, cmp);
            } else if (count[i] > 1) {
                insert_sort_core_<RandomAccessIterator, ValueType, Compare>(last[i-1], last[i], cmp);
            }
        }
    }
}

template <class RandomAccessIterator, class ValueType, class RadixByte, class Compare>
inline void radix_sort_entry_(RandomAccessIterator s, RandomAccessIterator e, ValueType*,
                              RadixByte rbyte, Compare cmp)
{
    if (e - s <= (int)kInsertSortThreshold)
        insert_sort_core_<RandomAccessIterator, ValueType, Compare>(s, e, cmp);
    else
        radix_sort_core_<RandomAccessIterator, ValueType, RadixByte,
                          Compare, RadixByte::nBytes - 1>(s, e, rbyte, cmp);
}

template <class RandomAccessIterator, class ValueType>
inline void radix_sort_entry_(RandomAccessIterator s, RandomAccessIterator e, ValueType *)
{
    if (ValueType(-1) > ValueType(0)) {
        radix_sort_entry_(s, e, (ValueType*)(0), RadixByteUnsigned<ValueType>(), std::less<ValueType>());
    } else {
        radix_sort_entry_(s, e, (ValueType*)(0), RadixByteSigned<ValueType>(), std::less<ValueType>());
    }
}

//================= INTERFACES ====================

template <class RandomAccessIterator, class RadixByte, class Compare>
inline void radix_sort(RandomAccessIterator s, RandomAccessIterator e, RadixByte rbyte, Compare cmp)
{
    typename std::iterator_traits<RandomAccessIterator>::value_type *dummy(0);
    radix_sort_entry_(s, e, dummy, rbyte, cmp);
}

template <class RandomAccessIterator>
inline void radix_sort(RandomAccessIterator s, RandomAccessIterator e)
{
    typename std::iterator_traits<RandomAccessIterator>::value_type *dummy(0);
    radix_sort_entry_(s, e, dummy);
}

}

#endif
