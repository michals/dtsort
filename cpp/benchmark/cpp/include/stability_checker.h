#pragma once

#include <iterator>
#include <iostream>
#include <string>

// Post-sort verification utilities.

namespace stability {

// Check that [begin, end) is sorted according to comparator cmp.
template<typename Iter, typename Cmp>
bool check_sorted(Iter begin, Iter end, Cmp cmp) {
    if (begin == end) return true;
    auto prev = begin;
    for (auto it = std::next(begin); it != end; ++it) {
        if (cmp(*it, *prev)) {
            return false;
        }
        prev = it;
    }
    return true;
}

// Check that [begin, end) is sorted using operator<.
template<typename Iter>
bool check_sorted(Iter begin, Iter end) {
    return check_sorted(begin, end, [](const auto& a, const auto& b) { return a < b; });
}

// Check sort stability: for adjacent equal-key elements, stability_tag must
// be in ascending order (preserving original insertion order).
//
// Requires: elements have a .stability_tag member, and the range is sorted
// by the element's natural ordering (operator<).
template<typename Iter>
bool check_stable(Iter begin, Iter end) {
    if (begin == end) return true;
    auto prev = begin;
    for (auto it = std::next(begin); it != end; ++it) {
        // If neither a < b nor b < a, they are equal-keyed
        if (!(*prev < *it) && !(*it < *prev)) {
            if (prev->stability_tag > it->stability_tag) {
                return false;
            }
        }
        prev = it;
    }
    return true;
}

// Combined check with diagnostic output
template<typename Iter, typename Cmp>
bool verify(Iter begin, Iter end, Cmp cmp, const std::string& label) {
    bool sorted_ok = check_sorted(begin, end, cmp);
    bool stable_ok = check_stable(begin, end);

    std::cout << "  " << label << ": "
              << (sorted_ok ? "sorted=PASS" : "sorted=FAIL") << "  "
              << (stable_ok ? "stable=PASS" : "stable=FAIL") << "\n";

    return sorted_ok && stable_ok;
}

} // namespace stability
