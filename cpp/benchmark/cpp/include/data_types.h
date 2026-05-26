#pragma once

#include <string>
#include <memory>
#include <vector>
#include <cstring>
#include <algorithm>

#include "metrics.h"

// ============================================================================
// 5 instrumented data types for benchmarking (see docs/benchmark.md §6)
//
// Each type has:
//   - A sort key
//   - A stability_tag (set by generator, verified post-sort)
//   - Instrumented comparison, move, and copy operations
// ============================================================================

// ---------------------------------------------------------------------------
// Type 1: IntElement — trivial cmp, trivial move (8 bytes)
// ---------------------------------------------------------------------------
template<bool Observed = true>
struct IntElement_ {
    int key;
    int stability_tag;

    IntElement_() : key(0), stability_tag(0) {}
    IntElement_(int k, int tag) : key(k), stability_tag(tag) {}

    // Move
    IntElement_(IntElement_&& o) noexcept : key(o.key), stability_tag(o.stability_tag) {
        if constexpr (Observed) metrics::count_move();
    }
    IntElement_& operator=(IntElement_&& o) noexcept {
        key = o.key;
        stability_tag = o.stability_tag;
        if constexpr (Observed) metrics::count_move();
        return *this;
    }

    // Copy
    IntElement_(const IntElement_& o) : key(o.key), stability_tag(o.stability_tag) {
        if constexpr (Observed) metrics::count_copy();
    }
    IntElement_& operator=(const IntElement_& o) {
        key = o.key;
        stability_tag = o.stability_tag;
        if constexpr (Observed) metrics::count_copy();
        return *this;
    }

    bool operator<(const IntElement_& o) const {
        if constexpr (Observed) metrics::count_comparison();
        return key < o.key;
    }
};

// IntElement: Instrumented version for algorithmic counts (Section 2, 3, 4 of report).
// Incurs a small performance tax for atomic counter increments.
using IntElement = IntElement_<true>;

// IntElementNaked: Zero-overhead version for wall-clock timing (Section 6 of report).
// Used to measure raw sorting speed without instrumentation noise.
using IntElementNaked = IntElement_<false>;

// ---------------------------------------------------------------------------
// Type 2: MultiKeyRecord — expensive cmp (multi-field string), moderate move
// ---------------------------------------------------------------------------
struct MultiKeyRecord {
    std::string last_name;
    std::string first_name;
    int department;
    double salary;
    int stability_tag;

    MultiKeyRecord() : department(0), salary(0.0), stability_tag(0) {}
    MultiKeyRecord(std::string ln, std::string fn, int dept, double sal, int tag)
        : last_name(std::move(ln)), first_name(std::move(fn)),
          department(dept), salary(sal), stability_tag(tag) {}

    // Move
    MultiKeyRecord(MultiKeyRecord&& o) noexcept
        : last_name(std::move(o.last_name)), first_name(std::move(o.first_name)),
          department(o.department), salary(o.salary), stability_tag(o.stability_tag) {
        metrics::count_move();
    }
    MultiKeyRecord& operator=(MultiKeyRecord&& o) noexcept {
        last_name = std::move(o.last_name);
        first_name = std::move(o.first_name);
        department = o.department;
        salary = o.salary;
        stability_tag = o.stability_tag;
        metrics::count_move();
        return *this;
    }

    // Copy
    MultiKeyRecord(const MultiKeyRecord& o)
        : last_name(o.last_name), first_name(o.first_name),
          department(o.department), salary(o.salary), stability_tag(o.stability_tag) {
        metrics::count_copy();
    }
    MultiKeyRecord& operator=(const MultiKeyRecord& o) {
        last_name = o.last_name;
        first_name = o.first_name;
        department = o.department;
        salary = o.salary;
        stability_tag = o.stability_tag;
        metrics::count_copy();
        return *this;
    }

    bool operator<(const MultiKeyRecord& o) const {
        metrics::count_comparison();
        if (last_name != o.last_name) return last_name < o.last_name;
        if (first_name != o.first_name) return first_name < o.first_name;
        if (department != o.department) return department < o.department;
        return salary < o.salary;
    }
};

// ---------------------------------------------------------------------------
// Type 3: WideRow — trivial cmp, expensive move (4KB payload)
// ---------------------------------------------------------------------------
struct WideRow {
    int key;
    char data[4096];
    int stability_tag;

    WideRow() : key(0), stability_tag(0) { std::memset(data, 0, sizeof(data)); }
    WideRow(int k, int tag) : key(k), stability_tag(tag) { std::memset(data, 0, sizeof(data)); }

    // Move (still must copy 4KB — no heap to steal)
    WideRow(WideRow&& o) noexcept : key(o.key), stability_tag(o.stability_tag) {
        std::memcpy(data, o.data, sizeof(data));
        metrics::count_move();
    }
    WideRow& operator=(WideRow&& o) noexcept {
        key = o.key;
        stability_tag = o.stability_tag;
        std::memcpy(data, o.data, sizeof(data));
        metrics::count_move();
        return *this;
    }

    // Copy
    WideRow(const WideRow& o) : key(o.key), stability_tag(o.stability_tag) {
        std::memcpy(data, o.data, sizeof(data));
        metrics::count_copy();
    }
    WideRow& operator=(const WideRow& o) {
        key = o.key;
        stability_tag = o.stability_tag;
        std::memcpy(data, o.data, sizeof(data));
        metrics::count_copy();
        return *this;
    }

    bool operator<(const WideRow& o) const {
        metrics::count_comparison();
        return key < o.key;
    }
};

// ---------------------------------------------------------------------------
// Type 4: SharedPtrElement — trivial cmp, moderate move (atomic refcount)
// ---------------------------------------------------------------------------
struct SharedPtrElement {
    int sort_key;
    std::shared_ptr<std::vector<int>> payload;
    int stability_tag;

    SharedPtrElement() : sort_key(0), stability_tag(0) {}
    SharedPtrElement(int k, std::shared_ptr<std::vector<int>> p, int tag)
        : sort_key(k), payload(std::move(p)), stability_tag(tag) {}

    // Move
    SharedPtrElement(SharedPtrElement&& o) noexcept
        : sort_key(o.sort_key), payload(std::move(o.payload)), stability_tag(o.stability_tag) {
        metrics::count_move();
    }
    SharedPtrElement& operator=(SharedPtrElement&& o) noexcept {
        sort_key = o.sort_key;
        payload = std::move(o.payload);
        stability_tag = o.stability_tag;
        metrics::count_move();
        return *this;
    }

    // Copy
    SharedPtrElement(const SharedPtrElement& o)
        : sort_key(o.sort_key), payload(o.payload), stability_tag(o.stability_tag) {
        metrics::count_copy();
    }
    SharedPtrElement& operator=(const SharedPtrElement& o) {
        sort_key = o.sort_key;
        payload = o.payload;
        stability_tag = o.stability_tag;
        metrics::count_copy();
        return *this;
    }

    bool operator<(const SharedPtrElement& o) const {
        metrics::count_comparison();
        return sort_key < o.sort_key;
    }
};

// ---------------------------------------------------------------------------
// Type 5: VarLengthString — moderate cmp, moderate move
// ---------------------------------------------------------------------------
struct VarLengthString {
    std::string data;
    int stability_tag;

    VarLengthString() : stability_tag(0) {}
    VarLengthString(std::string d, int tag) : data(std::move(d)), stability_tag(tag) {}

    // Move
    VarLengthString(VarLengthString&& o) noexcept
        : data(std::move(o.data)), stability_tag(o.stability_tag) {
        metrics::count_move();
    }
    VarLengthString& operator=(VarLengthString&& o) noexcept {
        data = std::move(o.data);
        stability_tag = o.stability_tag;
        metrics::count_move();
        return *this;
    }

    // Copy
    VarLengthString(const VarLengthString& o)
        : data(o.data), stability_tag(o.stability_tag) {
        metrics::count_copy();
    }
    VarLengthString& operator=(const VarLengthString& o) {
        data = o.data;
        stability_tag = o.stability_tag;
        metrics::count_copy();
        return *this;
    }

    bool operator<(const VarLengthString& o) const {
        metrics::count_comparison();
        return data < o.data;
    }
};

// ---------------------------------------------------------------------------
// Type 6: LongStringCommonPrefix — very expensive cmp (long shared prefix)
// ---------------------------------------------------------------------------
struct LongStringCommonPrefix {
    std::string data;
    int stability_tag;

    LongStringCommonPrefix() : stability_tag(0) {}
    LongStringCommonPrefix(std::string d, int tag) : data(std::move(d)), stability_tag(tag) {}

    // Move
    LongStringCommonPrefix(LongStringCommonPrefix&& o) noexcept
        : data(std::move(o.data)), stability_tag(o.stability_tag) {
        metrics::count_move();
    }
    LongStringCommonPrefix& operator=(LongStringCommonPrefix&& o) noexcept {
        data = std::move(o.data);
        stability_tag = o.stability_tag;
        metrics::count_move();
        return *this;
    }

    // Copy
    LongStringCommonPrefix(const LongStringCommonPrefix& o)
        : data(o.data), stability_tag(o.stability_tag) {
        metrics::count_copy();
    }
    LongStringCommonPrefix& operator=(const LongStringCommonPrefix& o) {
        data = o.data;
        stability_tag = o.stability_tag;
        metrics::count_copy();
        return *this;
    }

    bool operator<(const LongStringCommonPrefix& o) const {
        metrics::count_comparison();
        return data < o.data;
    }
};

// ---------------------------------------------------------------------------
// Type 7: ReversedDomain — realistic cmp (reversed DNS names)
// ---------------------------------------------------------------------------
struct ReversedDomain {
    std::string data;
    int stability_tag;

    ReversedDomain() : stability_tag(0) {}
    ReversedDomain(std::string d, int tag) : data(std::move(d)), stability_tag(tag) {}

    // Move
    ReversedDomain(ReversedDomain&& o) noexcept
        : data(std::move(o.data)), stability_tag(o.stability_tag) {
        metrics::count_move();
    }
    ReversedDomain& operator=(ReversedDomain&& o) noexcept {
        data = std::move(o.data);
        stability_tag = o.stability_tag;
        metrics::count_move();
        return *this;
    }

    // Copy
    ReversedDomain(const ReversedDomain& o)
        : data(o.data), stability_tag(o.stability_tag) {
        metrics::count_copy();
    }
    ReversedDomain& operator=(const ReversedDomain& o) {
        data = o.data;
        stability_tag = o.stability_tag;
        metrics::count_copy();
        return *this;
    }

    bool operator<(const ReversedDomain& o) const {
        metrics::count_comparison();
        return data < o.data;
    }
};

// ---------------------------------------------------------------------------
// Type 8: CaseInsensitiveString — non-trivial cmp logic
// ---------------------------------------------------------------------------
struct CaseInsensitiveString {
    std::string data;
    int stability_tag;

    CaseInsensitiveString() : stability_tag(0) {}
    CaseInsensitiveString(std::string d, int tag) : data(std::move(d)), stability_tag(tag) {}

    // Move
    CaseInsensitiveString(CaseInsensitiveString&& o) noexcept
        : data(std::move(o.data)), stability_tag(o.stability_tag) {
        metrics::count_move();
    }
    CaseInsensitiveString& operator=(CaseInsensitiveString&& o) noexcept {
        data = std::move(o.data);
        stability_tag = o.stability_tag;
        metrics::count_move();
        return *this;
    }

    // Copy
    CaseInsensitiveString(const CaseInsensitiveString& o)
        : data(o.data), stability_tag(o.stability_tag) {
        metrics::count_copy();
    }
    CaseInsensitiveString& operator=(const CaseInsensitiveString& o) {
        data = o.data;
        stability_tag = o.stability_tag;
        metrics::count_copy();
        return *this;
    }

    bool operator<(const CaseInsensitiveString& o) const {
        metrics::count_comparison();
        // Manual case-insensitive comparison
        auto size = std::min(data.size(), o.data.size());
        for (size_t i = 0; i < size; ++i) {
            char a = std::tolower(static_cast<unsigned char>(data[i]));
            char b = std::tolower(static_cast<unsigned char>(o.data[i]));
            if (a != b) return a < b;
        }
        return data.size() < o.data.size();
    }
};
