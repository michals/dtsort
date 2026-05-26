#pragma once
#include <array>
#include <vector>
#include <iomanip>
#include <sstream>
#include <string>
#include "consts.hh"
#include "debug.h"


/** to print vector */
template <typename T>
std::ostream& operator<< (std::ostream& out, const std::vector<T>& v) {
    out << "[";
    for(size_t i=0; i<v.size(); ++i) {
        out << v[i];
        if ((i+1) < v.size()) out << ", ";
    }
    out << ']';
    return out;
}


/** to print array */
template <typename T, size_t N>
std::ostream& operator<< (std::ostream& out, const std::array<T, N>& arr) {
    out << "[";
    for(size_t i=0; i<arr.size(); ++i) {
        out << arr[i];
        if ((i+1) < arr.size()) out << ", ";
    }
    out << ']';
    return out;
}

/***
 * Convert string like "0__01_010_0101" decimal treating input as ternary number
 * where first char is least significat, '0' is 0, '1' is 1 and '_' is 2.
 */
inline size_t ternaryToDec(const std::string& input) {
    size_t out = 0;
    for(char c: input) {
        switch(c) {
        case '_': out += 2; break;
        case '1': out += 1; break;
        }
        out *= 3;
    }
    return out / 3;
}

/** convert int to hex */
template <typename T >
std::string hexify(const T& i) {
    std::stringstream ss;
    ss << std::hex << i;
    return ss.str();
}
