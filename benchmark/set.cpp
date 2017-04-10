#include <iostream>
#include "bench_utils.hpp"

int main()
{
    using namespace std;

    cout << "Elements,Simple,Typed,Byte,Compact,Shrank\n";

    for (size_t size = 128; size < L2_CACHE_SIZE*4; size <<= 1) {
        uint64_t *increments = random_array<uint64_t>(size);
        uint64_t *order = new uint64_t[size];
        for (size_t i = 0; i < size; i++)
            order[i] = i;
        std::random_shuffle(order, order+size);

        int64_t *value = random_array<int64_t>(size);
        for (size_t i = 0; i < size; i++) {
            int inc = value[i] - value[i];
            if (inc < 0) inc = -inc;
            value[i] = (increments[i] + inc) < 64 ? inc : 0;
        }

        cout << size << ',';
        cout << bench_set<SimpleFenwickTree> (size, increments, order, value) << ','
             << bench_set<TypedFenwickTree>  (size, increments, order, value) << ','
             << bench_set<ByteFenwickTree>   (size, increments, order, value) << ','
             << bench_set<CompactFenwickTree>(size, increments, order, value) << ','
             << bench_set<ShrankFenwickTree> (size, increments, order, value) << '\n';

        delete[] increments;
        delete[] order;
        delete[] value;
    }

    return 0;
}
