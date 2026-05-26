#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <functional>
#include <benchmark/benchmark.h>
#include <chrono>

#include "../include/data_generators.h"
#include "../include/stability_checker.h"
#include "../include/metrics.h"
#include "../include/bench_common.h"

// ---------------------------------------------------------------------------
// Data Generation
// ---------------------------------------------------------------------------

template <typename T>
std::vector<T> generate_data(size_t n, uint64_t seed, InputPattern pattern = InputPattern::Random);

template <> inline std::vector<IntElement> generate_data<IntElement>(size_t n, uint64_t seed, InputPattern pattern) {
    return generators::generate_int<IntElement>(n, seed, pattern);
}
template <> inline std::vector<IntElementNaked> generate_data<IntElementNaked>(size_t n, uint64_t seed, InputPattern pattern) {
    return generators::generate_int<IntElementNaked>(n, seed, pattern);
}
template <> inline std::vector<MultiKeyRecord> generate_data<MultiKeyRecord>(size_t n, uint64_t seed, InputPattern pattern) {
    return generators::generate_multikey(n, seed, pattern);
}
template <> inline std::vector<WideRow> generate_data<WideRow>(size_t n, uint64_t seed, InputPattern pattern) {
    return generators::generate_wide(n, seed, pattern);
}
template <> inline std::vector<SharedPtrElement> generate_data<SharedPtrElement>(size_t n, uint64_t seed, InputPattern pattern) {
    return generators::generate_shared(n, seed, pattern);
}
template <> inline std::vector<VarLengthString> generate_data<VarLengthString>(size_t n, uint64_t seed, InputPattern pattern) {
    return generators::generate_varstring(n, seed, pattern);
}
template <> inline std::vector<LongStringCommonPrefix> generate_data<LongStringCommonPrefix>(size_t n, uint64_t seed, InputPattern pattern) {
    return generators::generate_long_prefix(n, seed, pattern);
}
template <> inline std::vector<ReversedDomain> generate_data<ReversedDomain>(size_t n, uint64_t seed, InputPattern pattern) {
    return generators::generate_reversed_domains(n, seed, pattern);
}
template <> inline std::vector<CaseInsensitiveString> generate_data<CaseInsensitiveString>(size_t n, uint64_t seed, InputPattern pattern) {
    return generators::generate_case_insensitive(n, seed, pattern);
}

// ---------------------------------------------------------------------------
// check_identical
// ---------------------------------------------------------------------------
template <typename T>
bool check_identical(const std::vector<T>& a, const std::vector<T>& b, const std::string& context) {
    if (a.size() != b.size()) {
        std::cout << "  [FAIL] Size mismatch in " << context << ": " << a.size() << " vs " << b.size() << "\n";
        return false;
    }
    for (size_t i = 0; i < a.size(); ++i) {
        if (a[i] < b[i] || b[i] < a[i]) { // Not equivalent
             // Dump first mismatch
             std::cout << "  [FAIL] Data mismatch at index " << i << " in " << context << "\n";
             return false;
        }
        if (a[i].stability_tag != b[i].stability_tag) {
             std::cout << "  [FAIL] Stability mismatch at index " << i << " in " << context 
                       << ": " << a[i].stability_tag << " vs " << b[i].stability_tag << "\n";
             return false;
        }
    }
    return true;
}

// ---------------------------------------------------------------------------
// validate_type_impl - Validates one type for one size
// ---------------------------------------------------------------------------
template <typename T, typename SortFunc, typename ReferenceSortFunc>
bool validate_type_impl(const std::string& impl_name, const std::string& type_name,
                        SortFunc test_sort, ReferenceSortFunc ref_sort, size_t size, InputPattern pattern = InputPattern::Random) 
{
    uint64_t seed = 12345 + size;
    auto data_test = generate_data<T>(size, seed, pattern);
    auto data_ref = generate_data<T>(size, seed, pattern); 

    metrics::reset();

    // Run Implementation under test
    test_sort(data_test.begin(), data_test.end());
    if (!stability::check_sorted(data_test.begin(), data_test.end()) ||
        !stability::check_stable(data_test.begin(), data_test.end())) {
        std::cout << "  [FAIL] " << impl_name << " " << type_name << " (N=" << size << ") failed sorted/stable checks.\n";
        return false;
    }

    // Run Reference (System std::stable_sort)
    ref_sort(data_ref.begin(), data_ref.end());

    // Compare
    if (!check_identical(data_test, data_ref, impl_name + " vs Reference " + type_name)) {
        return false;
    }

    return true;
}

// ---------------------------------------------------------------------------
// Generic Runner Template
// ---------------------------------------------------------------------------
template <typename SortImpl>
bool run_validation_suite(const std::string& name, const std::vector<size_t>& sizes) {
    bool all_passed = true;
    
    // We use the system's std::stable_sort as a reference for correctness
    auto ref_sort = [](auto f, auto l) { std::stable_sort(f, l); };

    for (size_t n : sizes) {
        if (!validate_type_impl<IntElement>(name, "IntElement", 
            [](auto f, auto l){ SortImpl::stable_sort(f, l); }, ref_sort, n)) all_passed = false;
            
        if (!validate_type_impl<MultiKeyRecord>(name, "MultiKey", 
            [](auto f, auto l){ SortImpl::stable_sort(f, l); }, ref_sort, n)) all_passed = false;

        if (!validate_type_impl<WideRow>(name, "WideRow", 
            [](auto f, auto l){ SortImpl::stable_sort(f, l); }, ref_sort, n)) all_passed = false;

        if (!validate_type_impl<SharedPtrElement>(name, "SharedPtr", 
            [](auto f, auto l){ SortImpl::stable_sort(f, l); }, ref_sort, n)) all_passed = false;

        if (!validate_type_impl<VarLengthString>(name, "VarString", 
            [](auto f, auto l){ SortImpl::stable_sort(f, l); }, ref_sort, n)) all_passed = false;

        if (!validate_type_impl<LongStringCommonPrefix>(name, "LongPrefix", 
            [](auto f, auto l){ SortImpl::stable_sort(f, l); }, ref_sort, n)) all_passed = false;

        if (!validate_type_impl<ReversedDomain>(name, "RevDomain", 
            [](auto f, auto l){ SortImpl::stable_sort(f, l); }, ref_sort, n)) all_passed = false;

        if (!validate_type_impl<CaseInsensitiveString>(name, "CaseInsens", 
            [](auto f, auto l){ SortImpl::stable_sort(f, l); }, ref_sort, n)) all_passed = false;
    }
    
    if (all_passed) std::cout << name << ": ALL PASS\n";
    else std::cout << name << ": FAILURES DETECTED\n";
    
    return all_passed;
}

// ---------------------------------------------------------------------------
// Counts Collection Logic
// ---------------------------------------------------------------------------

template <typename T, typename SortFunc>
RunMetrics measure_impl(const std::string& impl_name, const std::string& type_name, 
                        SortFunc sort_func, size_t size, const std::string& variant, int iterations, 
                        InputPattern pattern = InputPattern::Random) 
{
    uint64_t total_cmp = 0, total_move = 0, total_copy = 0;
    uint64_t total_dt_cmp = 0, total_dt_move = 0;
    size_t max_cmp = 0, max_move = 0;

    for (int i = 0; i < iterations; ++i) {
        auto data = generate_data<T>(size, 12345 + i, pattern);
        metrics::reset();
        sort_func(data.begin(), data.end());
        
        auto counters = metrics::get();
        total_cmp += counters.comparisons;
        total_move += counters.moves;
        total_copy += counters.copies;
        total_dt_cmp += counters.dtsort_comparisons;
        total_dt_move += counters.dtsort_moves;

        if (counters.comparisons > max_cmp) max_cmp = counters.comparisons;
        if (counters.moves > max_move) max_move = counters.moves;
    }
    
    RunMetrics m;
    m.impl_name = impl_name;
    m.type_name = type_name + (pattern == InputPattern::ReverseSorted ? "Reversed" :
                               pattern == InputPattern::Sorted ? "Sorted" : "");
    m.variant = variant;
    m.size = size;
    m.avg_comparisons = (double)total_cmp / iterations;
    m.max_comparisons = max_cmp;
    m.avg_dtsort_comparisons = (double)total_dt_cmp / iterations;
    m.avg_moves = (double)total_move / iterations;
    m.max_moves = max_move;
    m.avg_dtsort_moves = (double)total_dt_move / iterations;
    m.copies = (size_t)((double)total_copy / iterations); // Avg copies
    m.is_stock = (variant == "stock");
    m.pattern = pattern;
    
    return m;
}

template <typename SortImpl>
std::vector<RunMetrics> run_counts_suite(const std::string& name, const std::vector<size_t>& sizes, const std::string& variant, int iterations, InputPattern pattern = InputPattern::Random) {
    std::vector<RunMetrics> results;
    
    auto run_type = [&](auto dummy_type, const std::string& type_name) {
        using Type = decltype(dummy_type);
        for (size_t n : sizes) {
            results.push_back(measure_impl<Type>(name, type_name, 
                [](auto f, auto l){ SortImpl::stable_sort(f, l); }, n, variant, iterations, pattern));
        }
    };

    run_type(IntElement{}, "IntElement");
    return results;
}

template <typename SortImpl>
std::vector<RunMetrics> run_large_counts_suite(const std::string& name, const std::vector<size_t>& sizes, const std::string& variant, int iterations, InputPattern pattern = InputPattern::Random) {
    std::vector<RunMetrics> results;
    
    auto run_type = [&](auto dummy_type, const std::string& type_name) {
        using Type = decltype(dummy_type);
        for (size_t n : sizes) {
            results.push_back(measure_impl<Type>(name, type_name, 
                [](auto f, auto l){ SortImpl::stable_sort(f, l); }, n, variant, iterations, pattern));
        }
    };

    run_type(IntElement{}, "IntElement");
    return results;
}

template <typename T, typename SortImpl>
std::vector<RunMetrics> run_large_counts_for_type(const std::string& impl_name, const std::string& type_name, T dummy, const std::vector<size_t>& sizes, const std::string& variant, int iterations, InputPattern pattern) {
    std::vector<RunMetrics> results;
    for (size_t n : sizes) {
        results.push_back(measure_impl<T>(impl_name, type_name, 
            [](auto f, auto l){ SortImpl::stable_sort(f, l); }, n, variant, iterations, pattern));
    }
    return results;
}

// ---------------------------------------------------------------------------
// Weight Analysis Logic
// ---------------------------------------------------------------------------

template <typename T>
void measure_weight(const std::string& type_name) {
    const size_t NOPS = 20000; // 20k (Fast enough for WideRow/4KB while remaining statistically significant)
    uint64_t total_cmp_ns = 0, total_move_ns = 0;

    std::vector<T> data = generate_data<T>(NOPS, 12345);
    std::vector<T> data2 = generate_data<T>(NOPS, 54321);

    // Measure Comparison
    {
        auto start = std::chrono::high_resolution_clock::now();
        size_t dummy = 0;
        for (size_t i = 0; i < NOPS - 1; ++i) {
            if (data[i] < data[i+1]) dummy++;
        }
        auto end = std::chrono::high_resolution_clock::now();
        benchmark::DoNotOptimize(dummy);
        total_cmp_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
    }

    // Measure Move
    {
        auto start = std::chrono::high_resolution_clock::now();
        for (size_t i = 0; i < NOPS; ++i) {
            data[i] = std::move(data2[i]);
            benchmark::DoNotOptimize(data[i]);
        }
        auto end = std::chrono::high_resolution_clock::now();
        total_move_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
    }

    std::cout << type_name << ","
              << (double)total_cmp_ns / NOPS << ","
              << (double)total_move_ns / NOPS << "\n";
}

