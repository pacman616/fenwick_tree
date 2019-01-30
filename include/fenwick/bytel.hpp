#ifndef __FENWICK_LBYTE_HPP__
#define __FENWICK_LBYTE_HPP__

#include "fenwick_tree.hpp"
#include <iostream>
#include <fstream>
#include <sstream>

namespace hft::fenwick {

/**
 * class ByteL - byte compression and level ordered node layout.
 * @sequence: sequence of integers.
 * @size: number of elements.
 * @BOUND: maximum value that @sequence can store.
 *
 */
template <size_t BOUND> class ByteL : public FenwickTree {
public:
  static constexpr size_t BOUNDSIZE = log2(BOUND);
  static_assert(BOUNDSIZE >= 1 && BOUNDSIZE <= 64,
                "Leaves can't be stored in a 64-bit word");

protected:
  const size_t Size, Levels;
  DArray<uint8_t> Tree;
  unique_ptr<size_t[]> Level;

  std::array<uint64_t, 4096> addrprefix, addradd, addrfind;

public:
  ByteL(uint64_t sequence[], size_t size)
      : Size(size), Levels(lambda(size + 1) + 2),
        Level(make_unique<size_t[]>(Levels)) {
    Level[0] = 0;
    for (size_t i = 1; i < Levels; i++)
      Level[i] = ((size + (1 << (i - 1))) / (1 << i)) * heightsize(i - 1) +
                 Level[i - 1];

    Tree = DArray<uint8_t>(Level[Levels - 1] + 8); // +8 for safety

    for (size_t l = 0; l < Levels - 1; l++) {
      for (size_t node = 1 << l; node <= Size; node += 1 << (l + 1)) {
        size_t highpos = Level[l] + heightsize(l) * (node >> (l + 1));
        auint64_t &high = reinterpret_cast<auint64_t &>(Tree[highpos]);

        size_t sequence_idx = node - 1;
        uint64_t value = sequence[sequence_idx];
        for (size_t j = 0; j < l; j++) {
          sequence_idx >>= 1;
          size_t lowpos = Level[j] + heightsize(j) * sequence_idx;
          auint64_t low = *reinterpret_cast<auint64_t *>(&Tree[lowpos]);

          value += low & BYTE_MASK[heightsize(j)];
        }

        high &= ~BYTE_MASK[heightsize(l)];
        high |= value & BYTE_MASK[heightsize(l)];
      }
    }
  }

  virtual uint64_t prefix(size_t idx) {
    uint64_t sum = 0;

    while (idx != 0) {
      int height = rho(idx);
      size_t isize = heightsize(height);

      auint64_t &element = reinterpret_cast<auint64_t &>(
          Tree[Level[height] + (idx >> (1 + height)) * isize]);
      addrprefix[(uint64_t)(&element) % 4096]++;

      sum += element & BYTE_MASK[isize];
      idx = clear_rho(idx);
    }

    return sum;
  }

  virtual void add(size_t idx, int64_t inc) {
    while (idx <= Size) {
      int height = rho(idx);

      auint64_t &element = reinterpret_cast<auint64_t &>(
          Tree[Level[height] + (idx >> (1 + height)) * heightsize(height)]);
      addradd[(uint64_t)(&element) % 4096]++;

      element += inc;
      idx += mask_rho(idx);
    }
  }

  using FenwickTree::find;
  virtual size_t find(uint64_t *val) {
    size_t node = 0, idx = 0;

    for (size_t height = Levels - 2; height != SIZE_MAX; height--) {
      size_t isize = heightsize(height);
      size_t pos = Level[height] + idx * heightsize(height);

      idx <<= 1;

      if (pos >= Level[height + 1])
        continue;

      uint64_t element = *reinterpret_cast<auint64_t *>(&Tree[pos]);
      addrfind[(uint64_t)(&element) % 4096]++;
      uint64_t value = element & BYTE_MASK[isize];

      if (*val >= value) {
        idx++;
        *val -= value;
        node += 1 << height;
      }
    }

    return min(node, Size);
  }

  using FenwickTree::compFind;
  virtual size_t compFind(uint64_t *val) {
    size_t node = 0, idx = 0;

    for (size_t height = Levels - 2; height != SIZE_MAX; height--) {
      size_t isize = heightsize(height);
      size_t pos = Level[height] + idx * heightsize(height);

      idx <<= 1;

      if (pos >= Level[height + 1])
        continue;

      uint64_t element = *reinterpret_cast<auint64_t *>(&Tree[pos]);
      uint64_t value = (BOUND << height) - (element & BYTE_MASK[isize]);

      if (*val >= value) {
        idx++;
        *val -= value;
        node += 1 << height;
      }
    }

    return min(node, Size);
  }

  virtual size_t size() { return Size; }

  virtual size_t bitCount() {
    return sizeof(ByteL<BOUNDSIZE>) * 8 +
           Tree.bitCount() - sizeof(Tree) +
           Levels * sizeof(size_t) * 8;
  }

  ~ByteL() {
    std::ofstream fprefix(std::string("address_ByteL_prefix_") + STRINGIFY(MAGIC) + ".txt", std::ofstream::app);
    std::ofstream fadd(std::string("address_ByteL_add_") + STRINGIFY(MAGIC) + ".txt", std::ofstream::app);
    std::ofstream ffind(std::string("address_ByteL_find_") + STRINGIFY(MAGIC) + ".txt", std::ofstream::app);

    for (auto i: addrprefix) fprefix << i << "\n";
    for (auto i: addradd) fadd << i << "\n";
    for (auto i: addrfind) ffind << i << "\n";
  }

private:
  static inline size_t heightsize(size_t height) {
    return ((height + BOUNDSIZE - 1) >> 3) + 1;
  }
};

} // namespace hft::fenwick

#endif // __FENWICK_LBYTE_HPP__
