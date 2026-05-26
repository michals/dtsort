#pragma once

#include <benchmark/benchmark.h>
#include "cli.h"
#include "bench_common.h"
#include "dt_dispatch.h"
#include "shim_algorithm.h"

/**
 * @brief Stage: Wall-clock Benchmarking (Google Benchmark)
 * 
 * Performs high-fidelity wall-clock time measurements using the Google Benchmark 
 * framework. It supports statistical repetitions, various data types, and 
 * sizes ranging from small fixed inputs to large datasets.
 * 
 * Note: Custom dtsort flags (--stage, --types, etc.) are stripped before 
 * passing the arguments to benchmark::Initialize() to avoid collisions.
 */
template<typename SortImpl>
struct BenchmarkWrappers {
    static std::string impl_name;
    static std::string variant;

    template <typename T>
    static void register_small(const std::string& type_label, InputPattern pattern = InputPattern::Random) {
        std::string pattern_str = (pattern == InputPattern::Sorted) ? "sorted" : 
                                  (pattern == InputPattern::ReverseSorted) ? "reverse" : "random";
        std::string bench_name = impl_name + "/" + variant + "/" + type_label + "/" + pattern_str;
        
        auto* b = benchmark::RegisterBenchmark(bench_name.c_str(), [pattern](benchmark::State& state) {
            size_t n = state.range(0);
            auto data_orig = generate_data<T>(n, 12345, pattern);
            auto data = data_orig;
            for (auto _ : state) {
                data = data_orig;
                SortImpl::stable_sort(data.begin(), data.end());
                benchmark::DoNotOptimize(data.data());
            }
        });

        for (int i = 2; i <= MAX_DT_SIZE; ++i) {
            b->Arg(i);
        }
    }

    template <typename T>
    static void register_large(const std::string& type_label, const std::vector<size_t>& sizes, InputPattern pattern = InputPattern::Random) {
        std::string pattern_str = (pattern == InputPattern::Sorted) ? "sorted" : 
                                  (pattern == InputPattern::ReverseSorted) ? "reverse" : "random";
        std::string bench_name = impl_name + "/" + variant + "/" + type_label + "/" + pattern_str;
        
        auto* b = benchmark::RegisterBenchmark(bench_name.c_str(), [pattern](benchmark::State& state) {
            size_t n = state.range(0);
            auto data_orig = generate_data<T>(n, 12345, pattern);
            auto data = data_orig;
            for (auto _ : state) {
                data = data_orig;
                SortImpl::stable_sort(data.begin(), data.end());
                benchmark::DoNotOptimize(data.data());
            }
        });
        
        for (auto s : sizes) {
            b->Arg(s);
        }
    }
};

template<typename SortImpl> std::string BenchmarkWrappers<SortImpl>::impl_name;
template<typename SortImpl> std::string BenchmarkWrappers<SortImpl>::variant;

/**
 * @brief Dispatch function for wall-clock benchmarks.
 */
template<typename SortImpl>
int run_benchmark(int argc, char** argv, const CommandLine& cli, const std::string& impl_name, const std::string& variant) {
    BenchmarkWrappers<SortImpl>::impl_name = impl_name;
    BenchmarkWrappers<SortImpl>::variant = variant;

    auto types = cli.get_list("types");
    auto sizes_list = cli.get_list("sizes");
    std::vector<size_t> large_sizes;
    if (sizes_list.empty()) {
        large_sizes = {100, 300, 1000, 3000, 10000, 30000, 100000, 300000, 1000000};
    } else {
        for (const auto& s : sizes_list) large_sizes.push_back(std::stoull(s));
    }

    auto should_run = [&](const std::string& name) {
        if (types.empty()) return true;
        return shim::contains(types, name);
    };

    auto inputs = cli.get_list("inputs");
    std::vector<InputPattern> patterns;
    if (inputs.empty()) {
        patterns = {InputPattern::Random, InputPattern::ReverseSorted};
    } else {
        for (const auto& s : inputs) {
            if (s == "random") {
                patterns.push_back(InputPattern::Random);
            } else if (s == "reversed" || s == "reverse") {
                patterns.push_back(InputPattern::ReverseSorted);
            } else if (s == "sorted") {
                patterns.push_back(InputPattern::Sorted);
            }
        }
    }

    auto register_all_patterns = [&](auto dummy, const std::string& name, bool include_large = true) {
        if (!should_run(name)) return;
        
        bool run_small = !cli.has("large-only");
        bool run_large = include_large && !cli.has("small-only");

        if (run_small) {
            for (auto pattern : patterns) {
                BenchmarkWrappers<SortImpl>::template register_small<decltype(dummy)>(name, pattern);
            }
        }
        if (run_large) {
            for (auto pattern : patterns) {
                BenchmarkWrappers<SortImpl>::template register_large<decltype(dummy)>(name, large_sizes, pattern);
            }
        }
    };

    register_all_patterns(IntElementNaked{}, "IntNaked");
    register_all_patterns(MultiKeyRecord{}, "MultiKey");
    register_all_patterns(WideRow{}, "WideRow");
    register_all_patterns(LongStringCommonPrefix{}, "LongPrefix");
    register_all_patterns(ReversedDomain{}, "RevDomain");
    register_all_patterns(CaseInsensitiveString{}, "CaseInsens");

    // Filter out our custom flags so Google Benchmark doesn't complain about unrecognized arguments.
    // We translate --bench-iters=N to Google Benchmark's native --benchmark_repetitions=N.
    std::vector<std::string> storage;
    std::vector<char*> filtered_argv; 
    for(int i=0; i<argc; ++i) { 
        std::string arg = argv[i]; 
        if(arg.find("--bench-iters=") == 0) {
            storage.push_back("--benchmark_repetitions=" + arg.substr(14));
        } else if(arg.find("--stage=") != 0 && arg.find("--types=") != 0 && 
                  arg.find("--sizes=") != 0 && arg.find("--count-iters=") != 0 &&
                  arg.find("--small-only") != 0 && arg.find("--large-only") != 0 &&
                  arg.find("--inputs=") != 0 &&
                  arg.find("--iters=") != 0) { // Legacy iters support
            filtered_argv.push_back(argv[i]); 
        }
    } 
    // Add translated iters at the end
    for (const auto& s : storage) {
        filtered_argv.push_back(const_cast<char*>(s.c_str()));
    }

    int f_argc = (int)filtered_argv.size(); 
    char** f_argv = filtered_argv.data(); 

    benchmark::Initialize(&f_argc, f_argv);
    
    // Check for remaining unrecognized arguments (likely actual Google Benchmark errors)
    if (benchmark::ReportUnrecognizedArguments(f_argc, f_argv)) return 1;
    
    benchmark::RunSpecifiedBenchmarks();
    benchmark::Shutdown();
    return 0;
}
