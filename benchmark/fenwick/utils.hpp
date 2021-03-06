#ifndef __BENCHMARK_UTILS_HPP__
#define __BENCHMARK_UTILS_HPP__

#include <chrono>
#include <algorithm>
#include <random>

#include <fenwick/fixedf.hpp>
#include <fenwick/fixedl.hpp>
#include <fenwick/typel.hpp>
#include <fenwick/typef.hpp>
#include <fenwick/bytel.hpp>
#include <fenwick/bytef.hpp>
#include <fenwick/bitl.hpp>
#include <fenwick/bitf.hpp>


template<typename T> T* random_array(size_t size);
void inc_to_seq(std::uint64_t *inc, std::uint64_t *seq, std::size_t size);
template<typename T> double bench_prefix(std::size_t size, std::uint64_t increments[], std::uint64_t order[]);
template<typename T> double bench_add(std::size_t size, std::uint64_t increments[], std::uint64_t order[], std::int64_t values[]);
template<typename T> double bench_find(std::size_t size, std::uint64_t increments[], std::uint64_t elements[]);


template<typename T>
T* random_array(size_t size)
{
    using namespace std;
    static mt19937 mte;

    uniform_int_distribution<T> dist(0, 64);

    T *array = new T[size];
    for (size_t i = 0; i < size; i++)
        array[i] = dist(mte);

    return array;
}

void fill_with_random_values(std::uint64_t array[], std::size_t size)
{
    static std::mt19937 mte;

    std::uniform_int_distribution<std::uint64_t> dist(1, 60); // 0, 64

    for (std::size_t i = 0; i < size; i++)
        array[i] = dist(mte);
}

void inc_to_seq(std::uint64_t *inc, std::uint64_t *seq, std::size_t size)
{
    seq[0] = inc[0];
    for (std::size_t i = 1; i < size; i++)
        seq[i] = seq[i-1] + inc[i];
}


template<typename T>
double bench_prefix(std::size_t size, std::uint64_t increments[], std::uint64_t order[])
{
    using namespace std;
    uint64_t u = 0;

    T tree(increments, size);

    auto begin = chrono::high_resolution_clock::now();
    for (size_t i = 0; i < size; i++)
        u ^= tree.prefix(order[i]);
    auto end = chrono::high_resolution_clock::now();
    auto prefix = chrono::duration_cast<std::chrono::nanoseconds>(end-begin).count();

    const volatile uint64_t __attribute__((unused)) unused = u;

    const double c = 1. / size;
    return prefix * c;
}

template<typename T>
double bench_add(std::size_t size, std::uint64_t increments[], std::uint64_t order[], std::int64_t values[])
{
    using namespace std;

    T tree(increments, size);

    auto begin = chrono::high_resolution_clock::now();
    for (size_t i = 0; i < size; i++)
        tree.add(order[i], values[i]);
    auto end = chrono::high_resolution_clock::now();
    auto add = chrono::duration_cast<std::chrono::nanoseconds>(end-begin).count();

    const double c = 1. / size;
    return add * c;
}

template<typename T>
double bench_find(std::size_t size, std::uint64_t increments[], std::uint64_t elements[])
{
    using namespace std;
    uint64_t u = 0;

    T tree(increments, size);

    auto begin = chrono::high_resolution_clock::now();
    for (size_t i = 0; i < size; i++)
        u ^= tree.find(elements[i]);
    auto end = chrono::high_resolution_clock::now();
    auto find = chrono::duration_cast<std::chrono::nanoseconds>(end-begin).count();

    const volatile uint64_t __attribute__((unused)) unused = u;

    const double c = 1. / size;
    return find * c;
}


#endif // __BENCHMARK_UTILS_HPP__
