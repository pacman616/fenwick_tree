#ifndef __FENWICK_BYTEF_HPP__
#define __FENWICK_BYTEF_HPP__

#include "fenwick_tree.hpp"

namespace hft::fenwick {

/**
 * class ByteF - byte compression and classical node layout.
 * @sequence: sequence of integers.
 * @size: number of elements.
 * @BOUND: maximum value that @sequence can store.
 *
 */
template <size_t BOUND> class ByteF : public FenwickTree {
public:
  static constexpr size_t BOUNDSIZE = ceil_log2_plus1(BOUND);
  static_assert(BOUNDSIZE >= 1 && BOUNDSIZE <= 64, "Leaves can't be stored in a 64-bit word");

protected:
  size_t Size;
  DArray<uint8_t> Tree;

public:
  ByteF(uint64_t sequence[], size_t size) : Size(size), Tree(pos(size) + 8) {
    for (size_t i = 1; i <= size; i++) {
      auint64_t &element = reinterpret_cast<auint64_t &>(Tree[pos(i - 1)]);

      const size_t isize = bytesize(i);
      element &= ~BYTE_MASK[isize];
      element |= sequence[i - 1] & BYTE_MASK[isize];
    }

    for (size_t m = 2; m <= size; m <<= 1) {
      for (size_t idx = m; idx <= size; idx += m) {
        auint64_t &left = reinterpret_cast<auint64_t &>(Tree[pos(idx - 1)]);
        const auint64_t right = *reinterpret_cast<auint64_t *>(&Tree[pos(idx - m / 2 - 1)]);

        left += right & BYTE_MASK[bytesize(idx - m / 2)];
      }
    }
  }

  virtual uint64_t prefix(size_t idx) const {
    uint64_t sum = 0;

    while (idx != 0) {
      const uint64_t element = *reinterpret_cast<auint64_t *>(&Tree[pos(idx - 1)]);
      sum += element & BYTE_MASK[bytesize(idx)];
      idx = clear_rho(idx);
    }

    return sum;
  }

  virtual void add(size_t idx, int64_t inc) {
    while (idx <= Size) {
      reinterpret_cast<auint64_t &>(Tree[pos(idx - 1)]) += inc;
      idx += mask_rho(idx);
    }
  }

  using FenwickTree::find;
  virtual size_t find(uint64_t *val) const {
    size_t node = 0;

    for (size_t m = mask_lambda(Size); m != 0; m >>= 1) {
      if (node + m - 1 >= Size)
        continue;

      const uint64_t value =
          *reinterpret_cast<auint64_t *>(&Tree[pos(node + m - 1)]) & BYTE_MASK[bytesize(node + m)];

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
      if (node + m - 1 >= Size)
        continue;

      const uint64_t value =
          (BOUND << rho(node + m)) - (*reinterpret_cast<auint64_t *>(&Tree[pos(node + m - 1)]) &
                                      BYTE_MASK[bytesize(node + m)]);

      if (*val >= value) {
        node += m;
        *val -= value;
      }
    }

    return node;
  }

  virtual size_t size() const { return Size; }

  virtual size_t bitCount() const {
    return sizeof(ByteF<BOUNDSIZE>) * 8 + Tree.bitCount() - sizeof(Tree);
  }

private:
  static inline size_t bytesize(size_t idx) { return ((rho(idx) + BOUNDSIZE - 1) >> 3) + 1; }

  static inline size_t holes(size_t idx) {
#if HFT_HOLES == 1
    return 0;
#elif HFT_HOLES == 2
    return ((idx * 3) / (32 * 1024)) * 8;
#elif HFT_HOLES == 3
    return (idx / (64 * 1024)) * 8;
#else // HFT_HOLES = 0 (same as #ifndef)
    return (idx >> 14) * 8;
#endif
  }

  static inline size_t pos(size_t idx) {
    constexpr size_t NEXTBYTE = ((BOUNDSIZE - 1) | (8 - 1)) + 1;

    constexpr size_t SMALL = ((BOUNDSIZE - 1) >> 3) + 1;
    constexpr size_t MEDIUM = NEXTBYTE - BOUNDSIZE + 1;
    constexpr size_t LARGE = MEDIUM + 8;

    constexpr size_t MULTIPLIER = 8 - SMALL - 1;

    return idx * SMALL + (idx >> MEDIUM) + (idx >> LARGE) * MULTIPLIER + holes(idx);
  }

  friend std::ostream &operator<<(std::ostream &os, const ByteF<BOUND> &ft) {
    uint64_t nsize = hton((uint64_t)ft.Size);
    os.write((char *)&nsize, sizeof(uint64_t));

    return os << ft.Tree;
  }

  friend std::istream &operator>>(std::istream &is, ByteF<BOUND> &ft) {
    uint64_t nsize;
    is.read((char *)(&nsize), sizeof(uint64_t));
    ft.Size = ntoh(nsize);

    return is >> ft.Tree;
  }
};

} // namespace hft::fenwick

#endif // __FENWICK_BYTEF_HPP__
