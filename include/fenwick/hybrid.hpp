#ifndef __FENWICK_HYBRID_HPP__
#define __FENWICK_HYBRID_HPP__

#include "fenwick_tree.hpp"
#include <vector>

namespace hft {
    namespace fenwick {

        template <template<size_t> class TOP_TREE, template<size_t> class BOTTOM_TREE, size_t LEAF_MAXVAL, size_t BOTTOM_HEIGHT>
        class Hybrid : public FenwickTree
        {
        private:
            static constexpr size_t BOTTOM_ELEMENTS = (1ULL << BOTTOM_HEIGHT) - 1;

        protected:
            const size_t _size;
            std::vector<BOTTOM_TREE<LEAF_MAXVAL>> bottom_trees;
            TOP_TREE<LEAF_MAXVAL*(BOTTOM_ELEMENTS+1)> top_tree;

        public:
            Hybrid(uint64_t sequence[], size_t size) :
                _size(size),
                top_tree(build_top(sequence, size))
            {
                for (size_t i = 0; i < top_tree.size() + 1; i++) {
                    size_t length = (BOTTOM_ELEMENTS+1)*i + BOTTOM_ELEMENTS <= size ? BOTTOM_ELEMENTS : size & BOTTOM_ELEMENTS;
                    bottom_trees.push_back(BOTTOM_TREE<LEAF_MAXVAL>(sequence + (BOTTOM_ELEMENTS+1)*i, length));
                }

                for (size_t i = 0; i < top_tree.size(); i++)
                    top_tree.add(i+1, bottom_trees[i].prefix(bottom_trees[i].size()));
            }

            virtual uint64_t prefix(size_t idx) const
            {
                size_t top = idx >> BOTTOM_HEIGHT;
                size_t bottom = idx & BOTTOM_ELEMENTS;

                return top_tree.prefix(top) + bottom_trees[top].prefix(bottom);
            }

            virtual void add(size_t idx, int64_t inc)
            {
                size_t top = idx >> BOTTOM_HEIGHT;
                size_t bottom = idx & BOTTOM_ELEMENTS;

                if (bottom == 0) {
                    top_tree.add(top, inc);
                }
                else {
                    top_tree.add(top+1, inc);
                    bottom_trees[top].add(bottom, inc);
                }
            }

            using FenwickTree::find;
            virtual size_t find(uint64_t *val) const
            {
                size_t top = top_tree.size() != 0 ? top_tree.find(val) : 0;
                size_t bottom = top < bottom_trees.size() ? bottom_trees[top].find(val) : 0;

                return (top << BOTTOM_HEIGHT) + bottom;
            }

            using FenwickTree::compfind;
            virtual size_t compfind(uint64_t *val) const
            {
                size_t top = top_tree.size() != 0 ? top_tree.compfind(val) : 0;
                size_t bottom = top < bottom_trees.size() ? bottom_trees[top].compfind(val) : 0;

                return (top << BOTTOM_HEIGHT) + bottom;
            }

            virtual size_t size() const
            {
                return _size;
            }

            virtual size_t bit_count() const
            {
                size_t size = 0;
                for (auto &t : bottom_trees)
                    size += t.bit_count();

                return size
                    + sizeof(Hybrid<BOTTOM_TREE, TOP_TREE, LEAF_MAXVAL, BOTTOM_HEIGHT>)
                    + top_tree.bit_count();
            }

        private:
            static TOP_TREE<LEAF_MAXVAL*(BOTTOM_ELEMENTS+1)> build_top(const uint64_t sequence[], size_t size)
            {
                size_t length = size >> BOTTOM_HEIGHT;
                std::unique_ptr<uint64_t[]> subseq = std::make_unique<uint64_t[]>(length);

                for (size_t i = 1; i <= length; i++)
                    subseq[i-1] = sequence[i * (BOTTOM_ELEMENTS+1) - 1];

                return TOP_TREE<LEAF_MAXVAL*(BOTTOM_ELEMENTS+1)>(subseq.get(), length);
            }

        };

    }
}

#endif // __FENWICK_HYBRID_HPP__
