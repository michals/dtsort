#pragma once

#include <vector>
#include <random>
#include <string>
#include <memory>
#include <cstdint>
#include <algorithm>
#include <string_view>

#include "data_types.h"

// Deterministic random data generators for each benchmark type.
// The seed parameter ensures reproducibility.

enum class InputPattern {
    Random,
    Sorted,
    ReverseSorted
};

namespace generators {

// Helper: apply pattern to a vector
template <typename T>
inline void apply_pattern(std::vector<T>& v, InputPattern pattern) {
    if (pattern == InputPattern::Sorted) {
        std::sort(v.begin(), v.end());
    } else if (pattern == InputPattern::ReverseSorted) {
        std::sort(v.begin(), v.end(), [](const T& a, const T& b) { return b < a; });
    }
}

// Helper: random string of given length
inline std::string random_string(std::mt19937& rng, size_t len) {
    static const char charset[] = "abcdefghijklmnopqrstuvwxyz";
    std::uniform_int_distribution<int> dist(0, sizeof(charset) - 2);
    std::string s;
    s.reserve(len);
    for (size_t i = 0; i < len; ++i) {
        s.push_back(charset[dist(rng)]);
    }
    return s;
}

// --- IntElement ---
template <typename T = IntElement>
inline std::vector<T> generate_int(size_t n, uint64_t seed, InputPattern pattern = InputPattern::Random) {
    std::mt19937 rng(seed);
    std::uniform_int_distribution<int> dist(0, static_cast<int>(n));
    std::vector<T> v;
    v.reserve(n);
    for (size_t i = 0; i < n; ++i) {
        v.emplace_back(dist(rng), static_cast<int>(i));
    }
    apply_pattern(v, pattern);
    return v;
}

// --- MultiKeyRecord ---
inline std::vector<MultiKeyRecord> generate_multikey(size_t n, uint64_t seed, InputPattern pattern = InputPattern::Random) {
    std::mt19937 rng(seed);
    std::uniform_int_distribution<int> dept_dist(1, 20);
    std::uniform_real_distribution<double> sal_dist(30000.0, 200000.0);
    std::vector<MultiKeyRecord> v;
    v.reserve(n);
    for (size_t i = 0; i < n; ++i) {
        // Use short names (6-10 chars) for SSO-friendly strings
        auto ln = random_string(rng, 6 + rng() % 5);
        auto fn = random_string(rng, 4 + rng() % 5);
        v.emplace_back(std::move(ln), std::move(fn),
                       dept_dist(rng), sal_dist(rng),
                       static_cast<int>(i));
    }
    apply_pattern(v, pattern);
    return v;
}

// --- WideRow ---
inline std::vector<WideRow> generate_wide(size_t n, uint64_t seed, InputPattern pattern = InputPattern::Random) {
    std::mt19937 rng(seed);
    std::uniform_int_distribution<int> dist(0, static_cast<int>(n));
    std::vector<WideRow> v;
    v.reserve(n);
    for (size_t i = 0; i < n; ++i) {
        v.emplace_back(dist(rng), static_cast<int>(i));
    }
    apply_pattern(v, pattern);
    return v;
}

// --- SharedPtrElement ---
inline std::vector<SharedPtrElement> generate_shared(size_t n, uint64_t seed, InputPattern pattern = InputPattern::Random) {
    std::mt19937 rng(seed);
    std::uniform_int_distribution<int> dist(0, static_cast<int>(n));
    std::vector<SharedPtrElement> v;
    v.reserve(n);
    for (size_t i = 0; i < n; ++i) {
        auto payload = std::make_shared<std::vector<int>>(10, static_cast<int>(i));
        v.emplace_back(dist(rng), std::move(payload), static_cast<int>(i));
    }
    apply_pattern(v, pattern);
    return v;
}

// --- VarLengthString ---
inline std::vector<VarLengthString> generate_varstring(size_t n, uint64_t seed, InputPattern pattern = InputPattern::Random) {
    std::mt19937 rng(seed);
    std::vector<VarLengthString> v;
    v.reserve(n);
    for (size_t i = 0; i < n; ++i) {
        // 8-30 char strings — long enough to defeat SSO sometimes
        auto s = random_string(rng, 8 + rng() % 23);
        v.emplace_back(std::move(s), static_cast<int>(i));
    }
    apply_pattern(v, pattern);
    return v;
}

// --- LongStringCommonPrefix ---
inline std::vector<LongStringCommonPrefix> generate_long_prefix(size_t n, uint64_t seed, InputPattern pattern = InputPattern::Random) {
    std::mt19937 rng(seed);
    std::vector<LongStringCommonPrefix> v;
    v.reserve(n);
    // 64-character common prefix
    std::string prefix = "the_quick_brown_fox_jumps_over_the_lazy_dog_and_then_it_runs_away_";
    for (size_t i = 0; i < n; ++i) {
        auto s = prefix + random_string(rng, 8);
        v.emplace_back(std::move(s), static_cast<int>(i));
    }
    apply_pattern(v, pattern);
    return v;
}

// --- ReversedDomain ---
inline std::vector<ReversedDomain> generate_reversed_domains(size_t n, uint64_t seed, InputPattern pattern = InputPattern::Random) {
    std::mt19937 rng(seed);
    std::vector<ReversedDomain> v;
    v.reserve(n);
    static constexpr std::string_view tlds[] = {"com", "org", "net", "io", "edu"};
    static constexpr std::string_view domains[] = {"google", "github", "apache", "linux", "kernel", "microsoft"};
    constexpr size_t tlds_size = sizeof(tlds) / sizeof(tlds[0]);
    constexpr size_t domains_size = sizeof(domains) / sizeof(domains[0]);
    for (size_t i = 0; i < n; ++i) {
        std::string s = std::string(tlds[rng() % tlds_size]) + "." + 
                        std::string(domains[rng() % domains_size]) + "." + 
                        random_string(rng, 4 + rng() % 8);
        v.emplace_back(std::move(s), static_cast<int>(i));
    }
    apply_pattern(v, pattern);
    return v;
}

// --- CaseInsensitiveString ---
inline std::vector<CaseInsensitiveString> generate_case_insensitive(size_t n, uint64_t seed, InputPattern pattern = InputPattern::Random) {
    std::mt19937 rng(seed);
    std::vector<CaseInsensitiveString> v;
    v.reserve(n);
    for (size_t i = 0; i < n; ++i) {
        auto s = random_string(rng, 8 + rng() % 8);
        // Randomly capitalize characters
        for (char& c : s) {
            if (rng() % 2) c = std::toupper(static_cast<unsigned char>(c));
        }
        v.emplace_back(std::move(s), static_cast<int>(i));
    }
    apply_pattern(v, pattern);
    return v;
}

} // namespace generators
