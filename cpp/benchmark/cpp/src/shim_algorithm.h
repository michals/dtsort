#pragma once
#include <vector>
#include <string>

/** @namespace shim @brief Minimal algorithm shims to avoid including <algorithm> and causing namespace conflicts. */
namespace shim {
    /** @brief Simple implementation of std::find-like behavior for strings in vectors. */
    template<typename T, typename U>
    bool contains(const std::vector<T>& vec, const U& val) {
        for (const auto& item : vec) {
            if (item == val) return true;
        }
        return false;
    }

}
