#ifndef __COMMON_HPP__
#define __COMMON_HPP__

#include <cstdint>
#include <x86intrin.h>
#include <algorithm>
#include <memory>

namespace hft {

    using std::min;
    using std::unique_ptr; using std::make_unique;
    using std::size_t; using std::uint64_t; using std::uint32_t; using std::uint16_t; using std::uint8_t;

    /**
     * Aliased unsigned integers
     *
     * Strict aliasing rule: it's illegal to access the same memory location
     * with data of different types. If you have two pointers T* and a U*, the
     * compiler can assume they are not pointingthe same data. Accessing such a
     * data invokes undefined behavior.
     *
     * GCC __may_alias__ attribute is basically the opposite of the C 'restrict'
     * keywoard, it prevents the compiler to makes such assumptions. With this
     * types is now valid to access aliased data.
     *
     */
    using auint64_t = std::uint64_t __attribute__((__may_alias__));
    using auint32_t = std::uint32_t __attribute__((__may_alias__));
    using auint16_t = std::uint16_t __attribute__((__may_alias__));
    using  auint8_t =  std::uint8_t __attribute__((__may_alias__));

    /**
     * Bitmask array used in fenwick::ByteL and fenwick::ByteF
     *
     */
    static constexpr uint64_t BYTE_MASK[] = { 0x0ULL,
                                              0xFFULL,
                                              0xFFFFULL,
                                              0xFFFFFFULL,
                                              0xFFFFFFFFULL,
                                              0xFFFFFFFFFFULL,
                                              0xFFFFFFFFFFFFULL,
                                              0xFFFFFFFFFFFFFFULL,
                                              0xFFFFFFFFFFFFFFFFULL };

    /**
     * log2() - Static (i.e. computed at compile time) log2 roundup
     *
     */
    constexpr size_t log2(size_t n)
    {
        return ((n<2) ? 1 : 1+log2(n/2));
    }


    /**
     * Required for select64
     *
     */
    constexpr uint8_t kSelectInByte[2048] = {
        8, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0,
        1, 0, 2, 0, 1, 0, 5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0,
        2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 6, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0,
        1, 0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 5, 0, 1, 0, 2, 0, 1, 0,
        3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 7, 0,
        1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0,
        2, 0, 1, 0, 5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2, 0,
        1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 6, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
        4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 5, 0, 1, 0, 2, 0, 1, 0, 3, 0,
        1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 8, 8, 8, 1,
        8, 2, 2, 1, 8, 3, 3, 1, 3, 2, 2, 1, 8, 4, 4, 1, 4, 2, 2, 1, 4, 3, 3, 1, 3, 2,
        2, 1, 8, 5, 5, 1, 5, 2, 2, 1, 5, 3, 3, 1, 3, 2, 2, 1, 5, 4, 4, 1, 4, 2, 2, 1,
        4, 3, 3, 1, 3, 2, 2, 1, 8, 6, 6, 1, 6, 2, 2, 1, 6, 3, 3, 1, 3, 2, 2, 1, 6, 4,
        4, 1, 4, 2, 2, 1, 4, 3, 3, 1, 3, 2, 2, 1, 6, 5, 5, 1, 5, 2, 2, 1, 5, 3, 3, 1,
        3, 2, 2, 1, 5, 4, 4, 1, 4, 2, 2, 1, 4, 3, 3, 1, 3, 2, 2, 1, 8, 7, 7, 1, 7, 2,
        2, 1, 7, 3, 3, 1, 3, 2, 2, 1, 7, 4, 4, 1, 4, 2, 2, 1, 4, 3, 3, 1, 3, 2, 2, 1,
        7, 5, 5, 1, 5, 2, 2, 1, 5, 3, 3, 1, 3, 2, 2, 1, 5, 4, 4, 1, 4, 2, 2, 1, 4, 3,
        3, 1, 3, 2, 2, 1, 7, 6, 6, 1, 6, 2, 2, 1, 6, 3, 3, 1, 3, 2, 2, 1, 6, 4, 4, 1,
        4, 2, 2, 1, 4, 3, 3, 1, 3, 2, 2, 1, 6, 5, 5, 1, 5, 2, 2, 1, 5, 3, 3, 1, 3, 2,
        2, 1, 5, 4, 4, 1, 4, 2, 2, 1, 4, 3, 3, 1, 3, 2, 2, 1, 8, 8, 8, 8, 8, 8, 8, 2,
        8, 8, 8, 3, 8, 3, 3, 2, 8, 8, 8, 4, 8, 4, 4, 2, 8, 4, 4, 3, 4, 3, 3, 2, 8, 8,
        8, 5, 8, 5, 5, 2, 8, 5, 5, 3, 5, 3, 3, 2, 8, 5, 5, 4, 5, 4, 4, 2, 5, 4, 4, 3,
        4, 3, 3, 2, 8, 8, 8, 6, 8, 6, 6, 2, 8, 6, 6, 3, 6, 3, 3, 2, 8, 6, 6, 4, 6, 4,
        4, 2, 6, 4, 4, 3, 4, 3, 3, 2, 8, 6, 6, 5, 6, 5, 5, 2, 6, 5, 5, 3, 5, 3, 3, 2,
        6, 5, 5, 4, 5, 4, 4, 2, 5, 4, 4, 3, 4, 3, 3, 2, 8, 8, 8, 7, 8, 7, 7, 2, 8, 7,
        7, 3, 7, 3, 3, 2, 8, 7, 7, 4, 7, 4, 4, 2, 7, 4, 4, 3, 4, 3, 3, 2, 8, 7, 7, 5,
        7, 5, 5, 2, 7, 5, 5, 3, 5, 3, 3, 2, 7, 5, 5, 4, 5, 4, 4, 2, 5, 4, 4, 3, 4, 3,
        3, 2, 8, 7, 7, 6, 7, 6, 6, 2, 7, 6, 6, 3, 6, 3, 3, 2, 7, 6, 6, 4, 6, 4, 4, 2,
        6, 4, 4, 3, 4, 3, 3, 2, 7, 6, 6, 5, 6, 5, 5, 2, 6, 5, 5, 3, 5, 3, 3, 2, 6, 5,
        5, 4, 5, 4, 4, 2, 5, 4, 4, 3, 4, 3, 3, 2, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
        8, 8, 8, 3, 8, 8, 8, 8, 8, 8, 8, 4, 8, 8, 8, 4, 8, 4, 4, 3, 8, 8, 8, 8, 8, 8,
        8, 5, 8, 8, 8, 5, 8, 5, 5, 3, 8, 8, 8, 5, 8, 5, 5, 4, 8, 5, 5, 4, 5, 4, 4, 3,
        8, 8, 8, 8, 8, 8, 8, 6, 8, 8, 8, 6, 8, 6, 6, 3, 8, 8, 8, 6, 8, 6, 6, 4, 8, 6,
        6, 4, 6, 4, 4, 3, 8, 8, 8, 6, 8, 6, 6, 5, 8, 6, 6, 5, 6, 5, 5, 3, 8, 6, 6, 5,
        6, 5, 5, 4, 6, 5, 5, 4, 5, 4, 4, 3, 8, 8, 8, 8, 8, 8, 8, 7, 8, 8, 8, 7, 8, 7,
        7, 3, 8, 8, 8, 7, 8, 7, 7, 4, 8, 7, 7, 4, 7, 4, 4, 3, 8, 8, 8, 7, 8, 7, 7, 5,
        8, 7, 7, 5, 7, 5, 5, 3, 8, 7, 7, 5, 7, 5, 5, 4, 7, 5, 5, 4, 5, 4, 4, 3, 8, 8,
        8, 7, 8, 7, 7, 6, 8, 7, 7, 6, 7, 6, 6, 3, 8, 7, 7, 6, 7, 6, 6, 4, 7, 6, 6, 4,
        6, 4, 4, 3, 8, 7, 7, 6, 7, 6, 6, 5, 7, 6, 6, 5, 6, 5, 5, 3, 7, 6, 6, 5, 6, 5,
        5, 4, 6, 5, 5, 4, 5, 4, 4, 3, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
        8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 4, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
        8, 8, 8, 8, 8, 5, 8, 8, 8, 8, 8, 8, 8, 5, 8, 8, 8, 5, 8, 5, 5, 4, 8, 8, 8, 8,
        8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 6, 8, 8, 8, 8, 8, 8, 8, 6, 8, 8, 8, 6, 8, 6,
        6, 4, 8, 8, 8, 8, 8, 8, 8, 6, 8, 8, 8, 6, 8, 6, 6, 5, 8, 8, 8, 6, 8, 6, 6, 5,
        8, 6, 6, 5, 6, 5, 5, 4, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 7, 8, 8,
        8, 8, 8, 8, 8, 7, 8, 8, 8, 7, 8, 7, 7, 4, 8, 8, 8, 8, 8, 8, 8, 7, 8, 8, 8, 7,
        8, 7, 7, 5, 8, 8, 8, 7, 8, 7, 7, 5, 8, 7, 7, 5, 7, 5, 5, 4, 8, 8, 8, 8, 8, 8,
        8, 7, 8, 8, 8, 7, 8, 7, 7, 6, 8, 8, 8, 7, 8, 7, 7, 6, 8, 7, 7, 6, 7, 6, 6, 4,
        8, 8, 8, 7, 8, 7, 7, 6, 8, 7, 7, 6, 7, 6, 6, 5, 8, 7, 7, 6, 7, 6, 6, 5, 7, 6,
        6, 5, 6, 5, 5, 4, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
        8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
        8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 5, 8, 8, 8, 8, 8, 8, 8, 8,
        8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 6, 8, 8,
        8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 6, 8, 8, 8, 8, 8, 8, 8, 6, 8, 8, 8, 6,
        8, 6, 6, 5, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
        8, 8, 8, 8, 8, 8, 8, 8, 8, 7, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 7,
        8, 8, 8, 8, 8, 8, 8, 7, 8, 8, 8, 7, 8, 7, 7, 5, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
        8, 8, 8, 8, 8, 7, 8, 8, 8, 8, 8, 8, 8, 7, 8, 8, 8, 7, 8, 7, 7, 6, 8, 8, 8, 8,
        8, 8, 8, 7, 8, 8, 8, 7, 8, 7, 7, 6, 8, 8, 8, 7, 8, 7, 7, 6, 8, 7, 7, 6, 7, 6,
        6, 5, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
        8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
        8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
        8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
        8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 6,
        8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
        8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
        8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 7, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
        8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 7, 8, 8, 8, 8, 8, 8, 8, 8,
        8, 8, 8, 8, 8, 8, 8, 7, 8, 8, 8, 8, 8, 8, 8, 7, 8, 8, 8, 7, 8, 7, 7, 6, 8, 8,
        8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
        8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
        8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
        8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
        8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
        8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
        8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
        8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
        8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
        8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 7
    };

    /**
     * rho - Find the index of the least significant 1-bit in a word.
     * @word: Binary word.
     *
     * The Knuth's ruler function returns the number of trailing 0-bits in @word
     * starting from the least significant bit position; so, it returns 0 when
     * @word is 2^0 and it returns 63 when @word is 2^63.
     *
     * Its behavior in zero is undefined.
     *
     */
    inline int rho(uint64_t word)
    {
        return __builtin_ctzll(word);
    }

    /**
     * lambda - Find the index of the most significant 1-bit in a word.
     * @word: Binary word.
     *
     * The Knuth's lambda function is the dual of the rho function.
     *
     * Its behavior in zero is undefined.
     *
     */
    inline int lambda(uint64_t word)
    {
        return 63 - __builtin_clzll(word);
    }

    /**
     * clear_rho - Set to 0 the least significant 1-bit in a word.
     * @word: Binary word.
     *
     */
    inline uint64_t clear_rho(uint64_t word)
    {
        return word & (word - 1ULL);
    }

    /**
     * mask_rho - Bitmask where only the least significant 1-bit is set.
     * @word: Binary word.
     *
     * Compute 2^rho(word) for any @word that is not zero.
     *
     */
    inline uint64_t mask_rho(uint64_t word)
    {
        return word & (-word);
    }

    /**
     * mask_lambda - Bitmask where only the most significant 1-bit is set.
     * @word: Binary word.
     *
     * Undefined behavior when @word is zero.
     *
     */
    inline uint64_t mask_lambda(uint64_t word)
    {
        return 0x8000000000000000 >> __builtin_clzll(word);
    }

    /**
     * compact_bitmask - Generate a compact bitmask.
     * @count: Quantity of set bit.
     * @pos: Starting position.
     *
     * This fucntion returns a bitmask with @count 1-bits: every bit from @pos
     * to @pos+@count is set to one. If @pos is zero the bitmask has its @count
     * least significant bits setted to one.
     *
     */
    inline uint64_t compact_bitmask(size_t count, size_t pos)
    {
        return (-(count != 0ULL)) & (UINT64_MAX >> (64 - count)) << pos;
    }

    /**
     * popcount - Count the number of 1-bits in a word.
     * @word: Binary word.
     *
     */
    inline int popcount(uint64_t word)
    {
        return __builtin_popcountll(word);
    }

    /**
     * mround - Returns a number rounded to the desired power of two multiple.
     * @number: value to round up.
     * @multiple: power of two to which you want to round @number
     *
     */
    inline uint64_t mround(uint64_t number, uint64_t multiple)
    {
        return ((number-1) | (multiple-1)) + 1;
    }

    /**
     * select64 - Returns the index of the k-th 1-bit in the 64-bit word x.
     * @x: 64-bit word.
     * @k: 0-based rank (@k = 0 returns the position of the first 1-bit).
     *
     * Uses the broadword selection algorithm by Vigna [1], improved by Gog and
     * Petri [2] and Vigna [3]. Implementation taken from Folly [4].
     *
     * [1] Sebastiano Vigna. Broadword Implementation of Rank/Select Queries.
     *     WEA, 2008
     *
     * [2] Simon Gog, Matthias Petri. Optimized succinct data structures for
     *     massive data. Softw. Pract. Exper., 2014
     *
     * [3] Sebastiano Vigna. MG4J 5.2.1. http://mg4j.di.unimi.it/
     *
     * [4] Facebook Folly library: https://github.com/facebook/folly
     *
     */
    inline uint64_t select64(uint64_t x, uint64_t k)
    {
#ifndef __haswell__
        constexpr uint64_t kOnesStep4  = 0x1111111111111111ULL;
        constexpr uint64_t kOnesStep8  = 0x0101010101010101ULL;
        constexpr uint64_t kLAMBDAsStep8  = 0x80ULL * kOnesStep8;

        auto s = x;
        s = s - ((s & 0xA * kOnesStep4) >> 1);
        s = (s & 0x3 * kOnesStep4) + ((s >> 2) & 0x3 * kOnesStep4);
        s = (s + (s >> 4)) & 0xF * kOnesStep8;
        uint64_t byteSums = s * kOnesStep8;

        uint64_t kStep8 = k * kOnesStep8;
        uint64_t geqKStep8 = (((kStep8 | kLAMBDAsStep8) - byteSums) & kLAMBDAsStep8);
        uint64_t place = popcount(geqKStep8) * 8;
        uint64_t byteRank = k - (((byteSums << 8) >> place) & uint64_t(0xFF));
        return place + kSelectInByte[((x >> place) & 0xFF) | (byteRank << 8)];
#elif defined(__GNUC__) || defined(__clang__)
        // GCC and Clang won't inline the intrinsics.
        uint64_t result = uint64_t(1) << k;

        asm("pdep %1, %0, %0\n\t"
            "tzcnt %0, %0"
            : "+r"(result)
            : "r"(x));

        return result;
#else
        return _tzcnt_u64(_pdep_u64(1ULL << k, x));
#endif
    }

}

#endif // __COMMON_HPP__
