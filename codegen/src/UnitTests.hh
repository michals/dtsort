#pragma once
#include "debug.h"
#include "Matrix.hh"
#include "PartialMatrix.hh"
#include "PermMatrix.hh"

/**
 * Legacy unit tests previously scattered in production headers.
 */

namespace UnitTests {

inline void test_Matrix() {
    Debugn("test_Matrix()");
    const N_t N = 5;
    Matrix<N> m;
    Debugn("Created new Matrix()");
    Debugn(m.str_long());

    Debugn("Setting (1,0) and (2,4) to 1");
    m.set(1,0,true);
    m.set(2,4,true);
    Debugn(m.str_long());
    Debugn("Short representation is: " << m.str_short());
    Debugn("Lower only short representation is: " << m.str_short_lower());

    Debugn("Setting lower triangular to 0");
    m.setLowerTriangular(0);
    Debugn(m.str_long());

    Debugn("Setting upper triangular to 1");
    m.setUpperTriangular(1);
    Debugn(m.str_long());
}

inline void test_PartialMatrix() {
    PartialMatrix<4> pm;
    pm.set(1, 0, true);
    pm.set(2, 1, false);
    Debugn("test_PartialMatrix()");
    Debugn("PartialMatrix\n" << pm.str_short_lower());
    Debugn("PartialMatrix\n" << pm.str_long());
}

inline void test_PermMatrix4() {
    Debugn("test_PermMatrix4()");
    // Placeholder for legacy test
}

inline void run_all() {
    test_Matrix();
    test_PartialMatrix();
    test_PermMatrix4();
}

} // namespace UnitTests
