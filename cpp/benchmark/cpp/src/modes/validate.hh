#pragma once

#include <iostream>
#include <vector>
#include <string>
#include "cli.hh"
#include "bench_common.hh"
#include "validation_logic.hh"
#include "shim_algorithm.hh"

/**
 * @brief Stage: Validation
 * 
 * Verifies the correctness and stability of the sorting implementation.
 * Runs checks against a reference std::stable_sort and validates that 
 * dtsort maintains relative order for equal elements (stability).
 */
template<typename SortImpl>
int run_validate(const CommandLine& cli, const std::string& impl_name, const std::string& variant) {
    auto types = cli.get_list("types");
    auto sizes_str = cli.get("sizes");
    std::vector<size_t> sizes;
    if (sizes_str.empty()) {
        for (size_t i = 2; i <= MAX_DT_SIZE; ++i) sizes.push_back(i);
    } else {
        auto size_list = cli.get_list("sizes");
        for (const auto& s : size_list) sizes.push_back(std::stoull(s));
    }

    auto should_run = [&](const std::string& name) {
        if (types.empty()) return true;
        return shim::contains(types, name);
    };

    bool all_passed = true;
    
    auto ref_sort = [](auto f, auto l) { 
        std::stable_sort(f, l); 
    };
    
    std::string name = impl_name + "_" + variant;

    for (size_t n : sizes) {
        if (should_run("IntNaked") || should_run("IntElement")) {
            if (!validate_type_impl<IntElement>(name, "IntElement", [](auto f, auto l){ SortImpl::stable_sort(f, l); }, ref_sort, n)) all_passed = false;
        }
        if (should_run("MultiKey")) {
            if (!validate_type_impl<MultiKeyRecord>(name, "MultiKey", [](auto f, auto l){ SortImpl::stable_sort(f, l); }, ref_sort, n)) all_passed = false;
        }
        if (should_run("WideRow")) {
            if (!validate_type_impl<WideRow>(name, "WideRow", [](auto f, auto l){ SortImpl::stable_sort(f, l); }, ref_sort, n)) all_passed = false;
        }
        if (should_run("LongPrefix")) {
            if (!validate_type_impl<LongStringCommonPrefix>(name, "LongPrefix", [](auto f, auto l){ SortImpl::stable_sort(f, l); }, ref_sort, n)) all_passed = false;
        }
        if (should_run("RevDomain")) {
            if (!validate_type_impl<ReversedDomain>(name, "RevDomain", [](auto f, auto l){ SortImpl::stable_sort(f, l); }, ref_sort, n)) all_passed = false;
        }
        if (should_run("CaseInsens")) {
            if (!validate_type_impl<CaseInsensitiveString>(name, "CaseInsens", [](auto f, auto l){ SortImpl::stable_sort(f, l); }, ref_sort, n)) all_passed = false;
        }
    }

    if (all_passed) std::cout << name << ": ALL PASS\n";
    else std::cout << name << ": FAILURES DETECTED\n";

    return all_passed ? 0 : 1;
}
