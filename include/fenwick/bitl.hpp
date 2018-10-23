#ifndef __FENWICK_LBIT_HPP__
#define __FENWICK_LBIT_HPP__

#include "../common.hpp"
#include "fenwick_tree.hpp"

namespace hft {
    namespace fenwick {

        /**
         * class BitL - bit compression and level ordered node layout.
         * @sequence: sequence of integers.
         * @size: number of elements.
         * @LEAF_MAXVAL: maximum value that @sequence can store.
         *
         */
        template<size_t LEAF_MAXVAL>
        class BitL : public FenwickTree
        {
        public:
            static constexpr size_t LEAF_BITSIZE = log2(LEAF_MAXVAL);
            static_assert(LEAF_BITSIZE >= 1 && LEAF_BITSIZE <= 64, "Leaves can't be stored in a 64-bit word");

        protected:
            const size_t _size;
            DArray<uint8_t> tree;
            DArray<size_t> level;

        public:
            BitL(uint64_t sequence[], size_t size) :
                _size(size),
                level(size != 0 ? lambda(size+1)+2 : 1)
            {
                level[0] = 0;
                for (size_t i = 1; i < level.size(); i++)
                    level[i] = ((size + (1<<(i-1))) / (1<<i)) * (LEAF_BITSIZE-1+i) + level[i-1];

                const size_t levels = level.size() - 1;
                tree = DArray<uint8_t>(level[levels] / 8 + 4); // +4 to prevent segfault on the last element

                for (size_t l = 0; l < levels; l++) {
                    for (size_t node = 1<<l; node <= size; node += 1 << (l+1)) {
                        const size_t curr_bit_pos = level[l] + (LEAF_BITSIZE+l) * (node >> (l+1));
                        const size_t curr_shift = curr_bit_pos & 0b111;
                        const uint64_t curr_mask = compact_bitmask(LEAF_BITSIZE+l, curr_shift);
                        auint64_t * const curr_element = reinterpret_cast<auint64_t*>(&tree[curr_bit_pos/8]);

                        size_t sequence_idx = node-1;
                        uint64_t value = sequence[sequence_idx];
                        for (size_t j = 0; j < l; j++) {
                            sequence_idx >>= 1;

                            const size_t prev_bit_pos = level[j] + (LEAF_BITSIZE+j) * sequence_idx;
                            const size_t prev_shift = prev_bit_pos & 0b111;
                            const uint64_t prev_mask = compact_bitmask(LEAF_BITSIZE+j, prev_shift);
                            const auint64_t * const prev_element = reinterpret_cast<auint64_t*>(&tree[prev_bit_pos/8]);

                            value += (*prev_element & prev_mask) >> prev_shift;
                        }

                        *curr_element &= ~curr_mask;
                        *curr_element |= curr_mask & (value << curr_shift);
                    }
                }
            }

            virtual uint64_t prefix(size_t idx) const
            {
                uint64_t sum = 0ULL;
                size_t index = 0ULL;

                while (idx != index) {
                    index += mask_lambda(idx ^ index);

                    const int height = rho(index);
                    const size_t level_idx = index >> (1 + height);

                    const size_t bit_pos = level[height] + (LEAF_BITSIZE+height) * level_idx;
                    const size_t shift = bit_pos & 0b111;
                    const uint64_t mask = compact_bitmask(LEAF_BITSIZE+height, shift);
                    const auint64_t * const compact_element = reinterpret_cast<auint64_t*>(&tree[bit_pos/8]);

                    sum += (*compact_element & mask) >> shift;
                }

                return sum;
            }

            virtual void add(size_t idx, int64_t inc)
            {
                while (idx <= size()) {
                    const int height = rho(idx);
                    const size_t level_idx = idx >> (1 + height);
                    const size_t bit_pos = level[height] + (LEAF_BITSIZE+height) * level_idx;
                    const size_t shift = bit_pos & 0b111;
                    auint64_t * const compact_element = reinterpret_cast<auint64_t*>(&tree[bit_pos/8]);

                    *compact_element += inc << shift;

                    idx += mask_rho(idx);
                }
            }

            using FenwickTree::find;
            virtual size_t find(uint64_t *val) const
            {
                size_t node = 0, idx = 0;

                for (size_t height = level.size() - 2; height != SIZE_MAX; height--) {
                    const size_t bit_pos = level[height] + (LEAF_BITSIZE+height) * idx;
                    const auint64_t * const compact_element = reinterpret_cast<auint64_t*>(&tree[bit_pos/8]);
                    const size_t shift = bit_pos & 0b111;
                    const uint64_t mask = compact_bitmask(LEAF_BITSIZE+height, 0);

                    idx <<= 1;

                    if (bit_pos >= level[height+1]) continue;

                    uint64_t value = (*compact_element >> shift) & mask;

                    if (*val >= value) {
                        idx++;
                        *val -= value;
                        node += 1 << height;
                    }
                }

                // TODO: provare a togliere
                return node <= size() ? node : size();
            }

            using FenwickTree::compfind;
            virtual size_t compfind(uint64_t *val) const
            {
                size_t node = 0, idx = 0;

                for (size_t height = level.size() - 2; height != SIZE_MAX; height--) {
                    const size_t bit_pos = level[height] + (LEAF_BITSIZE+height) * idx;
                    const auint64_t * const compact_element = reinterpret_cast<auint64_t*>(&tree[bit_pos/8]);
                    const size_t shift = bit_pos & 0b111;
                    const uint64_t mask = compact_bitmask(LEAF_BITSIZE+height, 0);

                    idx <<= 1;

                    if (bit_pos >= level[height+1]) continue;

                    uint64_t value = (LEAF_MAXVAL << height) - ((*compact_element >> shift) & mask);

                    if (*val >= value) {
                        idx++;
                        *val -= value;
                        node += 1 << height;
                    }
                }

                return node <= size() ? node : size();
            }

            virtual size_t size() const
            {
                return _size;
            }

            virtual size_t bit_count() const
            {
                return sizeof(BitL<LEAF_BITSIZE>)*8
                    + tree.bit_count() - sizeof(tree)
                    + level.bit_count() - sizeof(level);
            }
        };

    }
}

#endif // __FENWICK_LBIT_HPP__
