#ifndef __TYPED_FENWICK_TREE_H__
#define __TYPED_FENWICK_TREE_H__

#include "fenwick_tree.hpp"

/**
 * class TypedFenwickTree
 * @tree8-16-32-64: Fenwick Tree data.
 * @size: Number of elements in the tree.
 * @levels: Number of levels of the tree.
 * @level_start: Lookup table, it store every starting level index.
 *
 * Each node is the smallest datatype capable of holding its data. It's supposed
 * to store increments up to 64.
 *
 * In each tree, the data is stored in a bottom-up level-order manner.
 */
class TypedFenwickTree : public FenwickTree
{
public:
    static constexpr std::size_t LEAF_BITSIZE = 7;

protected:
    std::unique_ptr<std::uint64_t[]> tree64;
    std::unique_ptr<std::uint32_t[]> tree32;
    std::unique_ptr<std::uint16_t[]> tree16;
    std::unique_ptr<std::uint8_t[]>  tree8;

    const std::size_t size;
    const std::size_t levels;
    std::unique_ptr<std::size_t[]> level_start;
    std::size_t type_ends[4] = {0};

public:
    TypedFenwickTree(std::uint64_t sequence[], std::size_t size);

    virtual std::uint64_t get(std::size_t idx) const;

    virtual void set(std::size_t idx, std::uint64_t inc);

    virtual std::size_t find(std::uint64_t val, bool complement=false) const;

    virtual std::size_t bit_count() const;

private:
    template <typename T, std::size_t start, std::size_t end>
    inline void fill_tree(T *tree, std::uint64_t sequence[]) {
        for (std::size_t l = start-LEAF_BITSIZE; l < levels && l <= end-LEAF_BITSIZE; l++) {
            for (std::size_t node = 1<<l; node <= size; node += 1 << (l+1)) {
                std::size_t sequence_idx = node-1;
                T value = sequence[sequence_idx];

                for (std::size_t j = 0; j < l && j <= 8-LEAF_BITSIZE; j++) {
                    sequence_idx >>= 1;
                    value += tree8[level_start[j] + sequence_idx];
                }
                for (std::size_t j = 8-LEAF_BITSIZE+1; j < l && j <= 16-LEAF_BITSIZE; j++) {
                    sequence_idx >>= 1;
                    value += tree16[level_start[j] + sequence_idx];
                }
                for (std::size_t j = 16-LEAF_BITSIZE+1; j < l && j <= 32-LEAF_BITSIZE; j++) {
                    sequence_idx >>= 1;
                    value += tree32[level_start[j] + sequence_idx];
                }
                for (std::size_t j = 32-LEAF_BITSIZE+1; j < l && j <= 64-LEAF_BITSIZE; j++) {
                    sequence_idx >>= 1;
                    value += tree64[level_start[j] + sequence_idx];
                }

                tree[level_start[l] + (node >> (l+1))] = value;
            }
        }
    }
};

#endif // __TYPED_FENWICK_TREE_H__
