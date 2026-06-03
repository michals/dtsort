#pragma once
#include "Digits.hh"
#include "Matrix.hh"

/**
 * Defines Permutation Comparison Matrix.
 * N parameter is a matrix size
 * For N=4, columns are (a,b,c,d) and rows are (a,b,c,d)
 * If we know that comp(a, b)==true then in row=b col=a we have comp=true.
 * */
template <N_t N> class PermMatrix : public Matrix<N> {
public:
  /** Permutation passed is a special case of Digits */
  typedef Digits<N> perm_t;

  /** Holds original permutation passed to c-tor this matrix is created from */
  const perm_t perm;

  /** Constructs Permutation comparison matrix from a given permutation */
  PermMatrix(const perm_t &permutation) : perm(permutation) {
    for (N_t row = 0; row < N; ++row) {
      for (N_t col = 0; col < N; ++col) {
        if (row > col) { // ONLY below diagonal!
          this->set(row, col, perm.data[row] < perm.data[col]);
        }
      }
    }

    //        Debug(this->str_short_lower());
    //        Debugn(" PermMatrix" << perm);
  }

  ~PermMatrix() = default;
};
