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
struct RadixTraitsUnsigned {
    static const int nBytes = sizeof(T);
    int operator() (const T &x, int k) { return (x >> (kRadixBits * k)) & kRadixMask; }
};

template<class T>
struct RadixTraitsSigned {
    static const int nBytes = sizeof(T);
    static const T kMSB = T(0x80) << ((sizeof(T) - 1) * 8);
    int operator() (const T &x, int k) {
        return ((x ^ kMSB) >> (kRadixBits * k)) & kRadixMask;
    }
};

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

template <class RandomIt, class ValueType, class RadixTraits, class Compare, int kWhichByte>
inline void radix_sort_core_(RandomIt s, RandomIt e, RadixTraits radix_traits, Compare cmp)
{
    RandomIt last_[kRadixBin + 1];
    RandomIt *last = last_ + 1;
    size_t count[kRadixBin] = {0};

    for (RandomIt i = s; i < e; ++i) {
        ++count[radix_traits(*i, kWhichByte)];
    }

    last_[0] = last_[1] = s;

    for (int i = 1; i < kRadixBin; ++i) {
        last[i] = last[i-1] + count[i-1];
    }

    for (int i = 0; i < kRadixBin; ++i) {
        RandomIt end = last[i-1] + count[i];
        if (end == e) { last[i] = e; break; }
        while (last[i] != end) {
            ValueType swapper = *last[i];
            int tag = radix_traits(swapper, kWhichByte);
            if (tag != i) {
                do {
                    std::swap(swapper, *last[tag]++);
                } while ((tag = radix_traits(swapper, kWhichByte)) != i);
                *last[i] = swapper;
            }
            ++last[i];
        }
    }

    if (kWhichByte > 0) {
        for (int i = 0; i < kRadixBin; ++i) {
            if (count[i] > kInsertSortThreshold) {
                radix_sort_core_<RandomIt, ValueType, RadixTraits, Compare,
                                  (kWhichByte > 0 ? (kWhichByte - 1) : 0)>
                                  (last[i-1], last[i], radix_traits, cmp);
            } else if (count[i] > 1) {
                insert_sort_core_<RandomIt, ValueType, Compare>(last[i-1], last[i], cmp);
            }
        }
    }
}

template <class RandomIt, class ValueType, class RadixTraits, class Compare>
inline void radix_sort_entry_(RandomIt s, RandomIt e, ValueType*,
                              RadixTraits radix_traits, Compare cmp)
{
    if (e - s <= (int)kInsertSortThreshold)
        insert_sort_core_<RandomIt, ValueType, Compare>(s, e, cmp);
    else
        radix_sort_core_<RandomIt, ValueType, RadixTraits,
                          Compare, RadixTraits::nBytes - 1>(s, e, radix_traits, cmp);
}

template <class RandomIt, class ValueType, class RadixTraits>
inline void radix_sort_entry_(RandomIt s, RandomIt e, ValueType* dummy,
                              RadixTraits radix_traits)
{
    return radix_sort_entry_(s, e, dummy, radix_traits, std::less<ValueType>());
}

template <class RandomIt, class ValueType>
inline void radix_sort_entry_(RandomIt s, RandomIt e, ValueType *)
{
    if (ValueType(-1) > ValueType(0)) {
        radix_sort_entry_(s, e, (ValueType*)(0), RadixTraitsUnsigned<ValueType>(), std::less<ValueType>());
    } else {
        radix_sort_entry_(s, e, (ValueType*)(0), RadixTraitsSigned<ValueType>(), std::less<ValueType>());
    }
}

//================= INTERFACES ====================

template <class RandomIt, class RadixTraits, class Compare>
inline void radix_sort(RandomIt s, RandomIt e, RadixTraits radix_traits, Compare cmp)
{
    typename std::iterator_traits<RandomIt>::value_type *dummy(0);
    radix_sort_entry_(s, e, dummy, radix_traits, cmp);
}

template <class RandomIt, class RadixTraits>
inline void radix_sort(RandomIt s, RandomIt e, RadixTraits radix_traits)
{
    typename std::iterator_traits<RandomIt>::value_type *dummy(0);
    radix_sort_entry_(s, e, dummy, radix_traits);
}

template <class RandomIt>
inline void radix_sort(RandomIt s, RandomIt e)
{
    typename std::iterator_traits<RandomIt>::value_type *dummy(0);
    radix_sort_entry_(s, e, dummy);
}

}

#endif
