#pragma once

#include <iostream>
#include "cli.h"
#include "bench_common.h"
#include "shim_algorithm.h"

/**
 * @brief Stage: Weight Analysis
 * 
 * Measures the raw CPU cost (nanoseconds) of basic operations (comparison and 
 * assignment/move) for each data type. This provides the 'weight' factors 
 * used to explain why dtsort gains vary across different types.
 * 
 * Output: CSV to stdout.
 */
inline int run_weight(const CommandLine& cli) {
    auto types = cli.get_list("types");
    auto should_run = [&](const std::string& name) {
        if (types.empty()) return true;
        return shim::contains(types, name);
    };

    std::cout << "type,cmp_ns,move_ns\n";
    if (should_run("IntNaked"))  measure_weight<IntElementNaked>("IntNaked");
    if (should_run("MultiKey"))  measure_weight<MultiKeyRecord>("MultiKey");
    if (should_run("WideRow"))   measure_weight<WideRow>("WideRow");
    if (should_run("LongPrefix")) measure_weight<LongStringCommonPrefix>("LongPrefix");
    if (should_run("RevDomain"))  measure_weight<ReversedDomain>("RevDomain");
    if (should_run("CaseInsens")) measure_weight<CaseInsensitiveString>("CaseInsens");

    return 0;
}
