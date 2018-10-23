#ifndef __FENWICK_BIT_HPP__
#define __FENWICK_BIT_HPP__

#include "../common.hpp"
#include "fenwick_tree.hpp"

namespace hft {
    namespace fenwick {

        /**
         * class BitF - bit compression and classical node layout.
         * @sequence: sequence of integers.
         * @size: number of elements.
         * @LEAF_MAXVAL: maximum value that @sequence can store.
         *
         */
        template<size_t LEAF_MAXVAL>
        class BitF : public FenwickTree
        {
        public:
            static constexpr size_t LEAF_BITSIZE = log2(LEAF_MAXVAL);
            static_assert(LEAF_BITSIZE >= 1 && LEAF_BITSIZE <= 64, "Leaves can't be stored in a 64-bit word");

        protected:
            const size_t _size;
            DArray<uint8_t> tree;

        public:
            /**
             * BitF - Build a FenwickTree given a sequence of increments.
             * @sequence: Sequence of increments.
             * @size: Number of elements stored by the sequence.
             *
             * Running time: O(length)
             */
            BitF(uint64_t sequence[], size_t size) :
                _size(size),
                tree(get_bitpos(size)/8 + 4) // +4 to prevent segfault the last element
            {
                for (size_t i = 1; i <= size; i++) {
                    const size_t bitpos = get_bitpos(i-1);
                    auint64_t * const element = reinterpret_cast<auint64_t*>(&tree[bitpos/8]);

                    const int bitsize = LEAF_BITSIZE + rho(i);
                    const size_t shift = bitpos & 0b111;
                    const uint64_t mask = compact_bitmask(bitsize, shift);

                    *element &= ~mask;
                    *element |= mask & (sequence[i-1] << shift);
                }

                for (size_t m = 2; m <= size; m <<= 1) {
                    for (size_t idx = m; idx <= size; idx += m) {
                        const size_t left_bitpos = get_bitpos(idx-1);
                        auint64_t * const left_element = reinterpret_cast<auint64_t*>(&tree[left_bitpos/8]);
                        const size_t left_shift = left_bitpos & 0b111;

                        const size_t right_bitpos = get_bitpos(idx - m/2 - 1);
                        auint64_t * const right_element = reinterpret_cast<auint64_t*>(&tree[right_bitpos/8]);
                        const size_t right_shift = right_bitpos & 0b111;

                        const int right_bitsize = LEAF_BITSIZE + rho(idx - m/2);
                        const uint64_t right_mask = compact_bitmask(right_bitsize, right_shift);

                        uint64_t value = (right_mask & *right_element) >> right_shift;
                        *left_element += value << left_shift;
                    }
                }
            }

            virtual uint64_t prefix(size_t idx) const
            {
                uint64_t sum = 0;

                while (idx != 0) {
                    const size_t bit_pos = get_bitpos(idx-1);
                    const auint64_t * const compact_element = reinterpret_cast<auint64_t*>(&tree[bit_pos/8]);

                    const int height = rho(idx);
                    const size_t shift = bit_pos & 0b111;
                    const uint64_t mask = compact_bitmask(LEAF_BITSIZE+height, shift);

                    sum += (*compact_element & mask) >> shift;
                    idx = clear_rho(idx);
                }

                return sum;
            }

            virtual void add(size_t idx, int64_t inc)
            {
                while (idx <= size()) {
                    const size_t bit_pos = get_bitpos(idx-1);
                    auint64_t * const compact_element = reinterpret_cast<auint64_t*>(&tree[bit_pos/8]);

                    const size_t shift = bit_pos & 0b111;
                    *compact_element += inc << shift;

                    idx += mask_rho(idx);
                }
            }

            using FenwickTree::find;
            virtual size_t find(uint64_t *val) const
            {
                size_t node = 0;

                for (size_t m = mask_lambda(size()); m != 0; m >>= 1) {
                    if (node+m-1 >= size()) continue;

                    const size_t bit_pos = get_bitpos(node+m-1);
                    const int height = rho(node+m);
                    const size_t shift = bit_pos & 0b111;
                    const uint64_t mask = compact_bitmask(LEAF_BITSIZE+height, 0);

                    uint64_t value = (*reinterpret_cast<auint64_t*>(&tree[bit_pos/8]) >> shift) & mask;

                    if (*val >= value) {
                        node += m;
                        *val -= value;
                    }
                }

                return node;
            }

            using FenwickTree::compfind;
            virtual size_t compfind(uint64_t *val) const
            {
                size_t node = 0;

                for (size_t m = mask_lambda(size()); m != 0; m >>= 1) {
                    if (node+m-1 >= size()) continue;

                    const size_t bit_pos = get_bitpos(node+m-1);
                    const int height = rho(node+m);
                    const size_t shift = bit_pos & 0b111;
                    const uint64_t mask = compact_bitmask(LEAF_BITSIZE+height, 0);

                    uint64_t value = (LEAF_MAXVAL << height)
                        - ((*reinterpret_cast<auint64_t*>(&tree[bit_pos/8]) >> shift) & mask);

                    if (*val >= value) {
                        node += m;
                        *val -= value;
                    }
                }

                return node;
            }

            virtual size_t size() const
            {
                return _size;
            }

            virtual size_t bit_count() const
            {
                return sizeof(BitF<LEAF_BITSIZE>)*8
                    + tree.bit_count() - sizeof(tree);
            }

        private:
            inline size_t get_bitpos(size_t n) const
            {
                return (LEAF_BITSIZE+1)*n - popcount(n);
            }

        };

    }
}

#endif // __FENWICK_BIT_HPP__
