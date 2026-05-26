#pragma once
#include <array>
#include <iostream>
#include <algorithm>
#include <string>
#include "consts.hh"


/***
 * Used to represent permutation or sequence of non-unique numbers less than N
 */
template<N_t N>
struct Digits {
    typedef std::array<N_t, N> data_t;
    data_t data;

    /***
     * Set digits based on it's index. (all variations with repetitions 0..N**N)
     * 0 -> 000
     * 1 -> 001
     * 2 -> 002
     * 3 -> 010
     * ...
     * 3**3 -> 333
     * Returns false if i>N**N
     */
    bool setVariation(uint64_t i) {
        for (int n=N-1; n>=0; --n) {
            data[n] = i % N;
            i /= N;
        }
        return !i;
    }

    /***
     * Sets this digits as (0, 1, 2, .. , N-1)
     */
    void setFirstPermutation() {
        for (N_t n=0; n<N; ++n) {
            data[n] = n;
        }
    }

    /***
     * Set digits to next permutation.
     * Returns false if there are no more bigger permutations.
     * Note: use setFirstPermutation() first to iterate over all permutations.
     */
    bool setNextPermutation() {
        return std::next_permutation(data.begin(), data.end());
    }

    const std::string decomposedPermutation() const {
        std::string out;
        std::bitset<N> done;
        for (N_t i=0; i<N; ++i) {
            if (done[i] || i==data[i]) continue;
            out += "X(";
            out += (char)('a' + i);
            done.set(i);
            for(auto next=data[i]; next != i; next=data[next]) {
                out += ',';
                out += (char)('a' + next);
                done.set(next);
            }
            out += ");";
        }
        return out;
    }

    size_t countMoves() const {
        size_t moves = 0;
        std::bitset<N> done;
        for (auto i = 0; i < N; ++i) {
            if (done[i] || i == data[i]) continue;
            size_t cycleLength = 1;
            done.set(i);
            for (auto next = data[i]; next != i; next = data[next]) {
                cycleLength++;
                done.set(next);
            }
            // A cycle of length k requires k+1 moves (including temp)
            moves += (cycleLength + 1);
        }
        return moves;
    }

};


/** Used to print Digits as "Digits(1234)" */
template<N_t N>
inline std::ostream& operator<< (std::ostream& out, const Digits<N>& digits) {
    out << "(";
    for(auto & elem : digits.data) {
        out << (int)elem;
    }
    return (out << ")");
}
