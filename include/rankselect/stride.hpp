#ifndef __RANKSELECT_STRIDE_HPP__
#define __RANKSELECT_STRIDE_HPP__

#include "rank_select.hpp"

namespace hft {
    namespace ranking {

        /**
         * Stride - Linear search on many words.
         * @bitvector: A bitvector of 64-bit words.
         * @length: The length (in words) of the bitvector.
         * @T: Underlining Fenwick tree with an ungiven <size_t> bound.
         * @WORDS: Length (in words) of the linear search stride.
         *
         */
        template <template<size_t> class T, size_t WORDS>
        class Stride : public RankSelect
        {
        private:
            static constexpr size_t LEAF_MAXVAL = 64 * WORDS;
            T<LEAF_MAXVAL> tree;
            DArray<uint64_t> _bitvector;

        public:
            Stride(uint64_t bitvector[], size_t length):
                tree(build_fenwick(bitvector, length)),
                _bitvector(DArray<uint64_t>(length))
            {
                std::copy_n(bitvector, length, _bitvector.get());
            }

            Stride(DArray<uint64_t> bitvector, size_t length)
                : tree(build_fenwick(bitvector.get(), length)),
                  _bitvector(std::move(bitvector)) {}

            virtual const uint64_t* bitvector() const
            {
                return _bitvector.get();
            }

            virtual size_t size() const
            {
                return _bitvector.size();
            }

            virtual uint64_t rank(size_t pos) const
            {
                size_t idx = pos/(64*WORDS);
                uint64_t value = tree.prefix(idx);

                for (size_t i = idx*WORDS; i < pos/64; i++)
                    value += popcount(_bitvector[i]);

                return value + popcount(_bitvector[pos/64] & ((1ULL << (pos % 64)) - 1));
            }

            virtual uint64_t rank(size_t from, size_t to) const
            {
                return rank(to) - rank(from);
            }

            virtual uint64_t rankZero(size_t pos) const
            {
                return pos - rank(pos);
            }

            virtual uint64_t rankZero(size_t from, size_t to) const
            {
                return (to - from) - rank(from, to);
            }

            virtual size_t select(uint64_t rank) const
            {
                const size_t idx = tree.find(&rank);

                for (size_t i = idx*WORDS; i < idx*WORDS+WORDS; i++) {
                    if (i >= _bitvector.size()) return SIZE_MAX;

                    const uint64_t rank_chunk = popcount(_bitvector[i]);
                    if (rank < rank_chunk)
                        return i*64 + select64(_bitvector[i], rank);
                    else
                        rank -= rank_chunk;
                }

                return SIZE_MAX;
            }

            virtual size_t selectZero(uint64_t rank) const
            {
                const size_t idx = tree.compfind(&rank);

                for (size_t i = idx*WORDS; i < idx*WORDS+WORDS; i++) {
                    if (i >= _bitvector.size()) return SIZE_MAX;

                    const uint64_t rank_chunk = popcount(~_bitvector[i]);
                    if (rank < rank_chunk)
                        return i*64 + select64(~_bitvector[i], rank);
                    else
                        rank -= rank_chunk;
                }

                return SIZE_MAX;
            }

            virtual uint64_t update(size_t index, uint64_t word)
            {
                const uint64_t old = _bitvector[index];
                _bitvector[index] = word;
                tree.add(index / WORDS + 1, popcount(word) - popcount(old));

                return old;
            }

            virtual bool set(size_t index) {
                const uint64_t old = _bitvector[index / 64];
                _bitvector[index / 64] |= uint64_t(1) << (index % 64);
                bool is_changed = _bitvector[index / 64] != old;
                tree.add(index / (WORDS * 64) + 1, is_changed);

                return !is_changed;
            }

            virtual bool clear(size_t index) {
                const uint64_t old = _bitvector[index / 64];
                _bitvector[index / 64] &= ~(uint64_t(1) << (index % 64));
                bool is_changed = _bitvector[index / 64] != old;
                tree.add(index / (WORDS * 64) + 1, -is_changed);

                return is_changed;
            }

            virtual bool toggle(size_t index) {
                const uint64_t old = _bitvector[index / 64];
                _bitvector[index / 64] ^= uint64_t(1) << (index % 64);
                tree.add(index / (WORDS * 64) + 1, _bitvector[index / 64] > old ? 1 : -1);

                return _bitvector[index / 64] > old ? 0 : 1;
            }

            virtual size_t bit_count() const
            {
                return sizeof(Stride<T, WORDS>)
                    + _bitvector.bit_count() - sizeof(_bitvector)
                    + tree.bit_count() - sizeof(tree);
            }

        private:
            static T<LEAF_MAXVAL> build_fenwick(const uint64_t bitvector[], size_t length)
            {
                uint64_t *sequence = new uint64_t[length/WORDS + 1]();
                for (size_t i = 0; i < length; i++)
                    sequence[i/WORDS] += popcount(bitvector[i]);

                T<LEAF_MAXVAL> tree(sequence, length/WORDS + 1);
                delete[] sequence;
                return tree;
            }
        };

    }
}

#endif // __RANKSELECT_STRIDE_HPP__