#pragma once
#include <array>
#include <bitset>
#include <vector>
#include "common.h"
#include "Digits.hh"
#include "Matrix.hh"
#include "debug.h"


/**
 * Defines Comparison Matrix that is not yet fully known
 * N parameter is a matrix size
 * For N=4, columns are (a,b,c,d) and rows are (a,b,c,d)
 * If we know that comp(a, b)==true then in row=b col=a we have comp=true.
 * */
template<N_t N>
class PartialMatrix: public Matrix<N>
{
public:
    typedef std::bitset<N*N> mask_t;

    /*** What cells are already known */
    mask_t mask;

    constexpr void set(const size_t& index, bool compareValue) {
        this->data.set(index, compareValue);
        mask.set(index, 1);
    }

    constexpr void set(const N_t& row, const N_t col, bool compareValue) {
        return set(Matrix<N>::rc2i(row, col), compareValue);
    }

    constexpr void unset(const size_t& index) {
        this->data.set(index, 0);
        mask.set(index, 0);
    }

    constexpr void unset(const N_t& row, const N_t col) {
        return unset(Matrix<N>::rc2i(row, col));
    }

    /** Constructs Permutation comparison matrix from a given permutation */
    PartialMatrix()
    {
        mask.reset();
        // Diagonal is always 0 (false) and known!
        for(N_t i=0; i<N; ++i) {
            mask.set(Matrix<N>::rc2i(i,i));
        }
    }

    ~PartialMatrix() = default;

    /**
     * @brief Generates string representation of this Matrix data
     * @param rowSeparator optional
     * @param diagonal display diagonal with 'x' ?
     * @return string representation of matrix data
     */
    const std::string str(
            std::string rowSeparator="",
            bool diagonal=false,
            bool extraSpace=false,
            size_t markIndex=-1) const {
        std::string out;
        size_t i = 0;
        for(N_t row=0; row<N; ++row) {
            if (rowSeparator=="\n" && extraSpace) {
                out += ('a'+row);
                out += ": ";
            }
            for(N_t col=0; col<N; ++col, ++i) {
                if (row==col) {
                    if (diagonal) {
                        out += 'x';
                        if (extraSpace) out += ' ';
                    }
                    continue;
                }
                if (i == markIndex) {
                    out += '?';
                } else {
                    out += ((mask[i]) ? ('0'+this->data[i]) : '.');
                }
                if (extraSpace) out += ' ';
            }
            if (row+1 < N) out += rowSeparator;
        }
        return out;
    }

    /** all cell data except diagonal in single line */
    inline const std::string str_short() const {
        return str("", false, false);
    }

    /***
     * Print only lower diagonal data without diagonal in single line
     * If assumed and index is provide, pretends index bit is set to assumed.
     */
    const std::string str_short_lower(size_t index=0, bool assume=false) const {
        std::string out;
        size_t i = 0;
        for(N_t row=0; row<N; ++row) {
            for(N_t col=0; col<N; ++col, ++i) {
                if (row>col) {
                    if (i==index) {
                        out += (char)('0' + assume);
                    } else {
                        out += ((mask[i]) ? ('0' + this->data[i]) : '_');
                    }
                }
            }
        }
        return out;
    }

    /** all cell data with diagonal as multiline human readable form */
    inline const std::string str_long() const {
        return str("\n", true, true);
    }

    /***
     * vector of cell indices (that can be mapped to ROW/COL)
     * where we don't know yet the compare(row, col).
     * We consider only cells below diagonal.
     */
    std::vector<size_t> unknownYet() {
        std::vector<size_t> output;
        auto target = ~mask & Matrix<N>::lower.data;
        for(size_t i=0; i<N*N; ++i) {
            if (target[i]) output.push_back(i);
        }
        return output;
    }

};


