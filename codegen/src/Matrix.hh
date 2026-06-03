#pragma once
#include "consts.hh"
#include <bitset>
#include <string>
#include <vector>

/***
 * Represents bit square matrix as bitset row by row
 * Baseclass for PermMatrix and CmpMatrix.
 */
template <N_t N> struct Matrix {
  typedef std::bitset<N * N> data_t;

  data_t data;

  /***
   * Convert row and column to internal bitset index.
   * Implementation detail: row-major order (row*N + col).
   *
   * We use a single std::bitset<N*N> to store the matrix.
   * This allows for extremely fast copying and comparison (just one or two
   * machine instructions).
   *
   * Note: We mainly use the lower triangular part (row > col) for comparisons.
   * The upper triangular part is implicit (comp(a,b) = !comp(b,a)), and
   * diagonal is always 0.
   */
  static constexpr size_t rc2i(const N_t &row, const N_t col) {
    return (row * N) + col;
  }

  static constexpr N_t i2r(size_t index) { return index / N; }
  static constexpr N_t i2c(size_t index) { return index % N; }

  /**
   * @brief Returns indices of comparisons used for pairwise pre-sorting: (0,1),
   * (2,3), etc.
   */
  static std::vector<size_t> getPreSortedIndices() {
    std::vector<size_t> indices;
    for (int i = 0; i + 1 < N; i += 2) {
      indices.push_back(rc2i(i + 1, i));
    }
    return indices;
  }

  /**
   * @brief Returns lower-triangle indices for all intra-group comparisons.
   *
   * For groups {5, 3} with N=8, returns all C(5,2) + C(3,2) = 10 + 3 = 13
   * indices. Each index corresponds to a comparison (row, col) where both
   * elements belong to the same group and row > col (lower triangle).
   */
  static std::vector<size_t>
  getGroupSortedIndices(const std::vector<int> &groups) {
    std::vector<size_t> indices;
    int offset = 0;
    for (int groupSize : groups) {
      for (int r = 1; r < groupSize; ++r) {
        for (int c = 0; c < r; ++c) {
          indices.push_back(rc2i(offset + r, offset + c));
        }
      }
      offset += groupSize;
    }
    return indices;
  }

  /**
   * @brief Set specific comparison in this matrix
   * @param row in this matrix
   * @param col in this matrix
   * @param compValue value of compare(input[row], input[col])
   */
  constexpr void set(const N_t &row, const N_t col, const bool compareValue) {
    data.set(rc2i(row, col), compareValue);
  }

  inline bool operator==(const Matrix &rhs) const { return data == rhs.data; }

  /**
   * @brief Generates string representation of this Matrix data
   * @param rowSeparator optional
   * @param diagonal display diagonal with 'x' ?
   * @return string representation of matrix data
   */
  const std::string str(std::string rowSeparator = "", bool diagonal = false,
                        bool extraSpace = false) const {
    std::string out;
    size_t i = 0;
    for (N_t row = 0; row < N; ++row) {
      if (rowSeparator == "\n" && extraSpace) {
        out += ('a' + row);
        out += ": ";
      }
      for (N_t col = 0; col < N; ++col, ++i) {
        if (row == col) {
          if (diagonal) {
            out += 'x';
            if (extraSpace)
              out += ' ';
          }
          continue;
        }
        out += '0' + data[i];
        if (extraSpace)
          out += ' ';
      }
      if (row + 1 < N)
        out += rowSeparator;
    }
    return out;
  }

  /** all cell data except diagonal in single line */
  inline const std::string str_short() const { return str("", false, false); }

  /** only lower diagonal data without diagonal in single line */
  const std::string str_short_lower() const {
    std::string out;
    size_t i = 0;
    for (N_t row = 0; row < N; ++row) {
      for (N_t col = 0; col < N; ++col, ++i) {
        if (row > col)
          out += ('0' + data[i]);
      }
    }
    return out;
  }

  /** all cell data with diagonal as multiline human readable form */
  inline const std::string str_long() const { return str("\n", true, true); }

  /***
   * Set this matrix to value for each cell above diagonal
   */
  Matrix &setUpperTriangular(bool value = true) {
    size_t i = 0;
    for (N_t row = 0; row < N; ++row) {
      for (N_t col = 0; col < N; ++col, ++i) {
        if (row < col)
          data.set(i, value);
      }
    }
    return *this;
  }

  /***
   * Set this matrix to value for each cell below diagonal
   */
  Matrix &setLowerTriangular(bool value = true) {
    size_t i = 0;
    for (N_t row = 0; row < N; ++row) {
      for (N_t col = 0; col < N; ++col, ++i) {
        if (row > col)
          data.set(i, value);
      }
    }
    return *this;
  }

  static Matrix<N> upper;
  static Matrix<N> lower;
};

template <N_t N>
Matrix<N> Matrix<N>::upper = Matrix<N>().setUpperTriangular(true);

template <N_t N>
Matrix<N> Matrix<N>::lower = Matrix<N>().setLowerTriangular(true);
