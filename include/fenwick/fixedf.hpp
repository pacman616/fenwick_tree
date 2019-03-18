#ifndef __FENWICK_NAIVE_HPP__
#define __FENWICK_NAIVE_HPP__

#include "fenwick_tree.hpp"

namespace hft::fenwick {

/**
 * class FixedF - no compression and classical node layout.
 * @sequence: sequence of integers.
 * @size: number of elements.
 * @BOUND: maximum value that @sequence can store.
 *
 */
template <size_t BOUND> class FixedF : public FenwickTree {
public:
  static constexpr size_t BOUNDSIZE = ceil_log2_plus1(BOUND);
  static_assert(BOUNDSIZE >= 1 && BOUNDSIZE <= 64,
                "Leaves can't be stored in a 64-bit word");

protected:
  DArray<uint64_t> Tree;
  size_t Size;
public:
  FixedF(uint64_t sequence[], size_t size) : Tree(pos(size) + 1), Size(size) {
    for(size_t j = 1; j <= size; j++)
      Tree[pos(j)] = sequence[j - 1];

    for (size_t m = 2; m <= size; m <<= 1) {
      for (size_t idx = m; idx <= size; idx += m)
        Tree[pos(idx)] += Tree[pos(idx - m / 2)];
    }
  }

  virtual uint64_t prefix(size_t idx) const {
    uint64_t sum = 0;

    while (idx != 0) {
      sum += Tree[pos(idx)];
      idx = clear_rho(idx);
    }

    return sum;
  }

  virtual void add(size_t idx, int64_t inc) {
    while (idx <= Size) {
      Tree[pos(idx)] += inc;
      idx += mask_rho(idx);
    }
  }

  using FenwickTree::find;
  virtual size_t find(uint64_t *val) const {
    size_t node = 0;

    for (size_t m = mask_lambda(Size); m != 0; m >>= 1) {
      if (node + m > Size) continue;

      uint64_t value = Tree[pos(node + m)];

      if (*val >= value) {
        node += m;
        *val -= value;
      }
    }

    return node;
  }

  using FenwickTree::compFind;
  virtual size_t compFind(uint64_t *val) const {
    size_t node = 0;

    for (size_t m = mask_lambda(Size); m != 0; m >>= 1) {
      if (node + m > Size) continue;

      uint64_t value = (BOUND << rho(node + m)) - Tree[pos(node + m)];

      if (*val >= value) {
        node += m;
        *val -= value;
      }
    }

    return node;
  }

  virtual size_t size() const { return Size; }

  virtual size_t bitCount() const {
    return sizeof(FixedF<BOUNDSIZE>) * 8 + Tree.bitCount() - sizeof(Tree);
  }

private:
  static inline size_t pos(size_t index) {
     return index + ( index * 3 ) / (16 * 1024);
  }
};

} // namespace hft::fenwick

#endif // __FENWICK_NAIVE_HPP__
