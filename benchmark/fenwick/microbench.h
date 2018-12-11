#include <iostream>
#include <iomanip>
#include <string>
#include <chrono>
#include <algorithm>
#include <random>

#include "../../include/fenwick.hpp"
#include "../../include/darray.hpp"

template<typename T>
void bench(const char* name, size_t size, size_t queries, std::default_random_engine re)
{
    using namespace std;
    using namespace std::chrono;
    using namespace hft::fenwick;

    chrono::high_resolution_clock::time_point begin, end;

    uint64_t u = 0;
    const double c = 1. / size;
    constexpr size_t LEAF_MAXVAL = 64, REPS = 5, IDXMID = (REPS-1)/2;
    uniform_int_distribution<uint64_t> seqdist(0, LEAF_MAXVAL);
    uniform_int_distribution<uint64_t> cumseqdist(0, LEAF_MAXVAL*size);
    uniform_int_distribution<size_t> idxdist(1, size);

    unique_ptr<uint64_t[]> sequence = make_unique<uint64_t[]>(size);
    for (size_t i = 0; i < size; i++) sequence[i] = seqdist(re);

    // constructor
    begin = chrono::high_resolution_clock::now();
    T fenwick(sequence.get(), size);
    end = chrono::high_resolution_clock::now();
    auto ctor = chrono::duration_cast<chrono::nanoseconds>(end-begin).count();
    cout << name << ": " << fenwick.bit_count() * c << " b/item\n";
    cout << "ctor: " << ctor * c << setw(12) << " ns/item" << endl;

    // find
    cout << "find: " << flush;
    vector<chrono::nanoseconds::rep> find;
    for (size_t r = 0; r < REPS; r++) {
        begin = chrono::high_resolution_clock::now();
        for (size_t i = 0; i < queries; i++)
            u ^= fenwick.find(cumseqdist(re));
        end = chrono::high_resolution_clock::now();
        find.push_back(duration_cast<chrono::nanoseconds>(end-begin).count());
    }
    sort(find.begin(), find.end());
    cout << find[IDXMID] * c << setw(12) << " ns/item" << endl;

    const volatile uint64_t __attribute__((unused)) unused = u;
}