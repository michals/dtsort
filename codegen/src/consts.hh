#pragma once
#include <cstdint>


/** Used to represent a single digit in PermMatrix, Digits, etc. */
typedef uint8_t N_t;

/** While generating dtsortX.hh the biggest X we want to support */
inline constexpr N_t MAX_N = 10;

/** Precalculated n**n for n=0..MAX_N */
constexpr const uint64_t VARIATIONS[] = {
    1, 1, 4, 27, 256, 3125, 46656, 823543, 16777216, 
    387420489ULL, 10000000000ULL
};

/** Precalculated n! for n=0..MAX_N */
constexpr const uint64_t FACTORIAL[] = { 
    1, 1, 2, 6, 24, 120, 720, 5040, 40320, 
    362880, 3628800 
};

// OEIS A036604: Minimum number of comparisons to sort n elements (N=0..10)
inline constexpr int OEIS_A036604[] = {0, 0, 1, 3, 5, 7, 10, 13, 16, 19, 22};

