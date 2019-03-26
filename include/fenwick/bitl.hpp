#ifndef __FENWICK_BITL_HPP__
#define __FENWICK_BITL_HPP__

#include "fenwick_tree.hpp"

namespace hft::fenwick {

/**
 * class BitL - bit compression and level ordered node layout.
 * @sequence: sequence of integers.
 * @size: number of elements.
 * @BOUND: maximum value that @sequence can store.
 *
 */
template <size_t BOUND> class BitL : public FenwickTree {
public:
  static constexpr size_t BOUNDSIZE = ceil_log2_plus1(BOUND);
  static_assert(BOUNDSIZE >= 1 && BOUNDSIZE <= 57, "Some nodes will span on multiple words");

protected:
  size_t Size, Levels;
  unique_ptr<size_t[]> Level;
  DArray<uint8_t> Tree;

public:
  BitL(uint64_t sequence[], size_t size)
      : Size(size), Levels(size != 0 ? lambda(size) + 2 : 1), Level(make_unique<size_t[]>(Levels)) {
    Level[0] = 0;
    for (size_t i = 1; i < Levels; i++)
      Level[i] = ((size + (1ULL << (i - 1))) / (1ULL << i)) * (BOUNDSIZE - 1 + i) + Level[i - 1];

    Tree = DArray<uint8_t>((Level[Levels - 1] >> 3) + 8); // +8 for safety

    for (size_t l = 0; l < Levels - 1; l++) {
      for (size_t node = 1ULL << l; node <= size; node += 1ULL << (l + 1)) {
        const size_t highpos = Level[l] + (BOUNDSIZE + l) * (node >> (l + 1));
        const size_t highshift = highpos & 0b111;
        const uint64_t highmask = compact_bitmask(BOUNDSIZE + l, highshift);
        auint64_t &high = reinterpret_cast<auint64_t &>(Tree[highpos >> 3]);

        size_t sequence_idx = node - 1;
        uint64_t value = sequence[sequence_idx];

        for (size_t j = 0; j < l; j++) {
          sequence_idx >>= 1;
          const size_t lowpos = Level[j] + (BOUNDSIZE + j) * sequence_idx;
          const size_t lowshift = lowpos & 0b111;
          const uint64_t lowmask = compact_bitmask(BOUNDSIZE + j, lowshift);
          const uint64_t low = *reinterpret_cast<auint64_t *>(&Tree[lowpos >> 3]);

          value += (low & lowmask) >> lowshift;
        }

        high &= ~highmask;
        high |= (value << highshift) & highmask;
      }
    }
  }

  virtual uint64_t prefix(size_t idx) const {
    uint64_t sum = 0;

    while (idx != 0) {
      const int height = rho(idx);
      const size_t pos = Level[height] + (idx >> (1 + height)) * (BOUNDSIZE + height);
      auint64_t element = *reinterpret_cast<auint64_t *>(&Tree[pos >> 3]);

      sum += bitextract(element, pos & 0b111, BOUNDSIZE + height);
      idx = clear_rho(idx);
    }

    return sum;
  }

  virtual void add(size_t idx, int64_t inc) {
    while (idx <= Size) {
      const int height = rho(idx);
      const size_t pos = Level[height] + (idx >> (1 + height)) * (BOUNDSIZE + height);
      auint64_t &element = reinterpret_cast<auint64_t &>(Tree[pos >> 3]);

      element += inc << (pos & 0b111);
      idx += mask_rho(idx);
    }
  }

  using FenwickTree::find;
  virtual size_t find(uint64_t *val) const {
    size_t node = 0, idx = 0;

    for (size_t height = Levels - 2; height != SIZE_MAX; height--) {
      const size_t pos = Level[height] + idx * (BOUNDSIZE + height);

      idx <<= 1;

      if (pos >= Level[height + 1])
        continue;

      const uint64_t element = *reinterpret_cast<auint64_t *>(&Tree[pos >> 3]);
      const uint64_t value = bitextract(element, pos & 0b111, BOUNDSIZE + height);

      if (*val >= value) {
        idx++;
        *val -= value;
        node += 1ULL << height;
      }
    }

    return min(node, Size);
  }

  using FenwickTree::compFind;
  virtual size_t compFind(uint64_t *val) const {
    size_t node = 0, idx = 0;

    for (size_t height = Levels - 2; height != SIZE_MAX; height--) {
      const size_t pos = Level[height] + idx * (BOUNDSIZE + height);

      idx <<= 1;

      if (pos >= Level[height + 1])
        continue;

      const uint64_t element = *reinterpret_cast<auint64_t *>(&Tree[pos >> 3]);
      const uint64_t value =
          (BOUND << height) - bitextract(element, pos & 0b111, BOUNDSIZE + height);

      if (*val >= value) {
        idx++;
        *val -= value;
        node += 1ULL << height;
      }
    }

    return min(node, Size);
  }

  virtual size_t size() const { return Size; }

  virtual size_t bitCount() const {
    return sizeof(BitL<BOUNDSIZE>) * 8 + Tree.bitCount() - sizeof(Tree) +
           Levels * sizeof(size_t) * 8;
  }

private:
  friend std::ostream &operator<<(std::ostream &os, const BitL<BOUND> &ft) {
    const uint64_t nsize = hton((uint64_t)ft.Size);
    os.write((char *)&nsize, sizeof(uint64_t));

    const uint64_t nlevels = hton((uint64_t)ft.Levels);
    os.write((char *)&nlevels, sizeof(uint64_t));

    for (size_t i = 0; i < ft.Levels; ++i) {
      const uint64_t nlevel = hton((uint64_t)ft.Level[i]);
      os.write((char *)&nlevel, sizeof(uint64_t));
    }

    return os << ft.Tree;
  }

  friend std::istream &operator>>(std::istream &is, BitL<BOUND> &ft) {
    uint64_t nsize;
    is.read((char *)(&nsize), sizeof(uint64_t));
    ft.Size = ntoh(nsize);

    uint64_t nlevels;
    is.read((char *)&nlevels, sizeof(uint64_t));
    ft.Levels = ntoh(nlevels);

    ft.Level = make_unique<size_t[]>(ft.Levels);
    for (size_t i = 0; i < ft.Levels; ++i) {
      uint64_t nlevel;
      is.read((char *)&nlevel, sizeof(uint64_t));
      ft.Level[i] = ntoh(nlevel);
    }

    return is >> ft.Tree;
  }
};

} // namespace hft::fenwick

#endif // __FENWICK_BITL_HPP__
