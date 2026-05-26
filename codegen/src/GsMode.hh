#pragma once
#include <string>
#include <vector>
#include <cctype>
#include <numeric>

/**
 * @brief Shared utilities for parsing and validating group-sorted (gs) mode strings.
 *
 * A gs mode string has the form "gsXYZ" where X, Y, Z are single digits representing
 * group sizes. For example:
 *   "gs53" → groups {5, 3} (total N=8)
 *   "gs42" → groups {4, 2} (total N=6)
 *   "gs55" → groups {5, 5} (total N=10)
 */

/** Parse "gs53" into {5, 3}. Returns empty vector if not a gs mode. */
inline std::vector<int> parseGsGroups(const std::string& mode) {
    if (mode.size() < 3 || mode.substr(0, 2) != "gs") return {};
    std::vector<int> groups;
    for (size_t i = 2; i < mode.size(); ++i) {
        if (!std::isdigit(mode[i])) return {};
        groups.push_back(mode[i] - '0');
    }
    return groups;
}

/** Check if mode string represents a valid gs mode */
inline bool isGsMode(const std::string& mode) {
    return !parseGsGroups(mode).empty();
}

/** Sum the group sizes in a gs mode string. Returns 0 if not a gs mode. */
inline int gsGroupSum(const std::string& mode) {
    auto groups = parseGsGroups(mode);
    return std::accumulate(groups.begin(), groups.end(), 0);
}
