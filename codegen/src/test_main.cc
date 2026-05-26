#include <iostream>
#include <vector>
#include <string>
#include <numeric>
#include <cstring>
#include <limits>
#include <fstream>

// Core headers
#include "consts.hh"
#include "debug.h"
#include "common.h"
#include "Digits.hh"
#include "Matrix.hh"
#include "PartialMatrix.hh"
#include "PermMatrix.hh"
#include "Solutions.hh"
#include "DecisionTree.hh"
#include "GsMode.hh"
#include "DTValidator.hh"
#include "UnitTests.hh"
#include "gen/DTGenerator.hh"


// =========================================================================================
// Verification Logic
// =========================================================================================

template<N_t N>
ExhaustiveMetrics verify_stats(bool checkVariations, std::string mode, bool quiet = false) {
    if (mode == "gs") {
        if (N <= 5) mode = "full";
        else if (N == 6) mode = "gs42";
        else if (N == 7) mode = "gs52";
        else if (N == 8) mode = "gs53";
        else if (N == 9) mode = "gs54";
        else if (N == 10) mode = "gs55";
    }
    if (!quiet) std::cout << "Verifying N=" << (int)N << " (mode=" << mode << ")... ";
    auto& solutions = Solutions<N>::getInstance();
    
    PartialMatrix<N> partial;
    std::vector<size_t> candidates;

    if (isGsMode(mode)) {
        auto groups = parseGsGroups(mode);
        auto gsIndices = Matrix<N>::getGroupSortedIndices(groups);
        for (size_t idx : gsIndices) {
            partial.set(idx, false);
        }
        for (size_t i = 0; i < solutions.all.size(); ++i) {
            bool ok = true;
            for (size_t idx : gsIndices) {
                if (solutions.all[i].data[idx]) {
                    ok = false;
                    break;
                }
            }
            if (ok) candidates.push_back(i);
        }
    } else if (mode == "ps") {
        auto m2Indices = Matrix<N>::getPreSortedIndices();
        for (size_t idx : m2Indices) {
            partial.set(idx, false); 
        }

        for (size_t i = 0; i < solutions.all.size(); ++i) {
            bool ok = true;
            for (size_t idx : m2Indices) {
                if (solutions.all[i].data[idx]) {
                    ok = false;
                    break;
                }
            }
            if (ok) candidates.push_back(i);
        }
    } else {
        candidates.resize(solutions.all.size());
        std::iota(candidates.begin(), candidates.end(), 0);
    }
    
    auto root = buildTree<N>(partial, candidates, solutions);
    
    if (!quiet) {
        // 1. Validate correctness (Path discovery)
        if (!validateTree(*root, solutions, candidates.size(), mode)) {
            std::cout << "FAILED (Correctness)\n";
            exit(1);
        }

        // 2. Validate Sorting (Actual swap results)
        if (!validateSorting(*root, solutions, mode)) {
            std::cout << "FAILED (Sorting)\n";
            exit(1);
        }

        // 3. Validate Variations (Optional, duplicates)
        if (checkVariations) {
            if (!validateVariations(*root, solutions, mode)) {
                std::cout << "FAILED (Variations)\n";
                exit(1);
            }
        }
    }
    
    auto metrics = getExhaustiveMetrics(*root, solutions, mode);

    if (!quiet) {
        // 4. Check Depth vs OEIS
        TreeStats stats = getTreeStats(*root);
        int expectedMax = OEIS_A036604[N];
        
        int totalMaxDepth = stats.maxDepth;
        if (mode == "ps") {
            totalMaxDepth += Matrix<N>::getPreSortedIndices().size();
        } else if (isGsMode(mode)) {
            // For gs mode, total comparisons = DT comparisons + pre-sort comparisons
            // Pre-sort comparisons = sum of OEIS_A036604[K] for each group of size K
            auto groups = parseGsGroups(mode);
            for (int g : groups) {
                totalMaxDepth += OEIS_A036604[g];
            }
        }
        bool optimal = (totalMaxDepth == expectedMax);
        
        std::cout << "OK. TotalDepth=" << totalMaxDepth;
        if (mode == "ps") {
            std::cout << " (DT:" << stats.maxDepth << " + PS:" << Matrix<N>::getPreSortedIndices().size() << ") ";
        } else if (isGsMode(mode)) {
            auto groups = parseGsGroups(mode);
            int preSortCmp = 0;
            for (int g : groups) preSortCmp += OEIS_A036604[g];
            std::cout << " (DT:" << stats.maxDepth << " + GS:" << preSortCmp << ") ";
        } else {
            std::cout << " ";
        }
        if (optimal) std::cout << "[OPTIMAL] ";
        else std::cout << "[SUBOPTIMAL] (OptMax=" << expectedMax << ") ";
        
        std::cout << "Avg=" << stats.avgDepth() << "\n";
    }

    return metrics;
}

void run_check_all(bool extended, bool variations, const std::string& mode) {
    verify_stats<2>(variations, mode);
    verify_stats<3>(variations, mode);
    verify_stats<4>(variations, mode);
    verify_stats<5>(variations, mode);
    verify_stats<6>(variations, mode);
    verify_stats<7>(variations, mode);
    verify_stats<8>(variations, mode);
    
    if (extended) {
        verify_stats<9>(variations, mode);
    } else {
        std::cout << "Skipping N=9,10 (use --check-extended to include)\n";
    }
    
    std::cout << "\nAll verifications PASSED.\n";
}

void run_csv_metrics(const std::string& mode, const std::string& path = "", int max_n = 8) {
    std::ostream* out = &std::cout;
    std::ofstream file;
    if (!path.empty()) {
        file.open(path);
        if (!file.is_open()) {
            std::cerr << "Error: Could not open " << path << " for writing.\n";
            exit(1);
        }
        out = &file;
    }

    *out << "size,avg_cmp,max_cmp,avg_move,max_move\n";
    if (max_n < 2) max_n = 2;
    if (max_n > 10) max_n = 10;

    for (int i = 2; i <= max_n; ++i) {
        ExhaustiveMetrics m;
        switch (i) {
            case 2: m = verify_stats<2>(false, mode, true); break;
            case 3: m = verify_stats<3>(false, mode, true); break;
            case 4: m = verify_stats<4>(false, mode, true); break;
            case 5: m = verify_stats<5>(false, mode, true); break;
            case 6: m = verify_stats<6>(false, mode, true); break;
            case 7: m = verify_stats<7>(false, mode, true); break;
            case 8: m = verify_stats<8>(false, mode, true); break;
            case 9: m = verify_stats<9>(false, mode, true); break;
            case 10: m = verify_stats<10>(false, mode, true); break;
        }
        *out << i << "," << m.avgCmp << "," << m.maxCmp << "," << m.avgMove << "," << m.maxMove << "\n";
    }
}

void print_help() {
    std::cout << "Usage: dtsort-verify [options]\n";
    std::cout << "Options:\n";
    std::cout << "  --check           Run statistical verification for specific N (use with -n)\n";
    std::cout << "  --check-all       Run statistical verification for N=2..8\n";
    std::cout << "  --check-extended  Run statistical verification for N=2..9 (slow)\n";
    std::cout << "  --check-variations Also sort all N^N variations (slower)\n";
    std::cout << "  --unit-tests      Run legacy internal unit tests\n";
    std::cout << "  --mode <mode>     Mode to verify: full, ps, gs<digits> (e.g., gs53) [default: full]\n";
    std::cout << "  --csv [path]      Output exhaustive metrics in CSV format (N=2..8, or 2..max_n if -n is specified). Optional [path] for file output.\n";
    std::cout << "  -n <N>            Specific N for --check or maximum N for --csv\n";
    std::cout << "  -h, --help        Show this help\n";
}

int main(int argc, char **argv) {
    if (argc < 2) {
        print_help();
        return 1;
    }

    bool variations = false;
    bool runUnitTests = false;
    bool csvMode = false;
    std::string csvPath = "";
    std::string mode = "full";
    int n = -1;
    bool checkMode = false;
    bool checkAll = false;
    bool checkExtended = false;

    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "--check-variations") == 0) {
            variations = true;
        } else if (strcmp(argv[i], "--unit-tests") == 0) {
            runUnitTests = true;
        } else if (strcmp(argv[i], "--csv") == 0) {
            csvMode = true;
            if (i + 1 < argc && argv[i + 1][0] != '-') {
                csvPath = argv[++i];
            }
        } else if (strcmp(argv[i], "--mode") == 0) {
            if (i + 1 < argc) {
                mode = argv[++i];
            }
        } else if (strcmp(argv[i], "-n") == 0) {
            if (i + 1 < argc) {
                n = std::atoi(argv[++i]);
            } else {
                std::cerr << "Error: -n requires an argument.\n";
                return 1;
            }
        } else if (strcmp(argv[i], "--check") == 0) {
            checkMode = true;
        } else if (strcmp(argv[i], "--check-all") == 0) {
            checkAll = true;
        } else if (strcmp(argv[i], "--check-extended") == 0) {
            checkExtended = true;
        } else if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
            print_help();
            return 0;
        }
    }

    if (mode != "full" && mode != "ps" && mode != "gs" && !isGsMode(mode)) {
        std::cerr << "Error: Invalid mode '" << mode << "'. Use 'full', 'ps', 'gs', or 'gs<digits>' (e.g., gs53).\n";
        return 1;
    }

    if (runUnitTests) {
        UnitTests::run_all();
        if (argc == 2) return 0;
    }

    if (csvMode) {
        run_csv_metrics(mode, csvPath, (n != -1) ? n : 8);
        return 0;
    }

    if (checkAll) {
        run_check_all(false, variations, mode);
        return 0;
    }
    if (checkExtended) {
        run_check_all(true, variations, mode);
        return 0;
    }

    if (checkMode && n != -1) {
        switch (n) {
            case 2: verify_stats<2>(variations, mode); break;
            case 3: verify_stats<3>(variations, mode); break;
            case 4: verify_stats<4>(variations, mode); break;
            case 5: verify_stats<5>(variations, mode); break;
            case 6: verify_stats<6>(variations, mode); break;
            case 7: verify_stats<7>(variations, mode); break;
            case 8: verify_stats<8>(variations, mode); break;
            case 9: verify_stats<9>(variations, mode); break;
            case 10: verify_stats<10>(variations, mode); break;
            default:
                std::cerr << "N=" << n << " not supported for verification (try 2-10).\n";
                return 1;
        }
        return 0;
    }

    print_help();
    return 1;
}
