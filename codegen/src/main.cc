#include <iostream>
#include <vector>
#include <string>
#include <numeric>
#include <cstring>
#include <filesystem>

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
#include "gen/DTGenerator.hh"
#include "gen/CppGenerator.hh"

// =========================================================================================
// Generic Engine
// =========================================================================================

template<N_t N>
void generate_sorter(const std::string& baseName, const std::string& mode) {
    std::string fullName = baseName + "_" + mode;
    std::cout << std::endl << "Generating " << fullName << " (mode=" << mode << ") for N=" << (int)N << "...\n";
    
    // 1. Get Solutions
    std::cout << "Initializing Solutions..." << std::flush;
    auto& solutions = Solutions<N>::getInstance();
    std::cout << " Done. (" << solutions.all.size() << " perms)\n";
    
    // 2. Build or Load Tree
    std::string dtPath = "output/" + fullName + ".dt";
    std::unique_ptr<DTNode<N>> root;
    
    // Check if DT exists
    std::ifstream dtIn(dtPath);
    if (dtIn.good()) {
        dtIn.close();
        std::cout << "  Found existing " << dtPath << ", loading..." << std::flush;
        root = DTGenerator<N>::load(dtPath);
        if (root) {
            std::cout << " Done. (Loaded from cache)\n";
        } else {
            std::cout << " Failed to load. Rebuilding...\n";
        }
    }
    
    if (!root) {
        PartialMatrix<N> partial;
        std::vector<size_t> candidates;

        if (isGsMode(mode)) {
            // --- Group-Sorted Pre-sorting ---
            // We assume each group is already internally sorted.
            auto groups = parseGsGroups(mode);
            auto gsIndices = Matrix<N>::getGroupSortedIndices(groups);
            for (size_t idx : gsIndices) {
                partial.set(idx, false);
            }

            for (size_t i = 0; i < solutions.all.size(); ++i) {
                bool ok = true;
                for (size_t idx : gsIndices) {
                    if (solutions.all[i].data[idx]) { // If intra-group element is out of order, skip
                        ok = false;
                        break;
                    }
                }
                if (ok) candidates.push_back(i);
            }
            std::cout << "  Building Decision Tree (Group-Sorted " << mode << ", candidates=" << candidates.size() << ")..." << std::flush;
        } else {
            // Full search
            candidates.resize(solutions.all.size());
            std::iota(candidates.begin(), candidates.end(), 0);
            std::cout << "  Building Decision Tree (Full, candidates=" << candidates.size() << ")..." << std::flush;
        }
        
        root = buildTree<N>(partial, candidates, solutions);
        std::cout << " Done.\n";

        // 3. Save Tree (only if we built it)
        std::cout << "  Saving tree to " << dtPath << "...\n";
        std::ofstream dtOut(dtPath);
        DTGenerator<N> dtGen;
        dtGen.generate(dtOut, *root, solutions);
    }
    
    // Compute stats
    TreeStats stats = getTreeStats(*root);
    int expectedMax = OEIS_A036604[N];
    bool optimal = (stats.maxDepth == (size_t)expectedMax);
    
    std::cout << "  [Stats]\n";
    std::cout << "    Comparisons: [" << stats.minDepth << ".." << stats.maxDepth << "] (avg " << stats.avgDepth() << ")\n";
    std::cout << "    Nodes:       " << stats.nodes << " (leaves " << stats.leaves << ")\n";
    std::cout << "    Optimality:  " << (optimal ? "YES" : "NO") << " (Theory: " << expectedMax << " @ OEIS A036604)\n";
    std::cout << "    Path Length: " << stats.minDepth << " (min) / " << stats.maxDepth << " (max)\n";
    
    // 4. Generate C++
    // Compute specific path lengths for sorted and reverse inputs
    Digits<N> sortedPerm;
    sortedPerm.setFirstPermutation();
    int sortedPathLen = getPathLength(*root, sortedPerm);

    Digits<N> reversePerm;
    for (int i = 0; i < N; ++i) reversePerm.data[i] = N - 1 - i;
    int reversePathLen = getPathLength(*root, reversePerm);

    std::cout << "    Sorted path:   " << sortedPathLen << "\n";
    std::cout << "    Reverse path:  " << reversePathLen << "\n";

    std::string hhPath = "include/" + fullName + ".hh";
    std::cout << "  Generating C++ code to " << hhPath << "...\n";
    std::ofstream hhOut(hhPath);
    CppGenerator<N> cppGen;
    cppGen.mode = mode;
    cppGen.stats = stats;
    cppGen.sortedPathLen = sortedPathLen;
    cppGen.reversePathLen = reversePathLen;
    cppGen.generate(hhOut, *root, solutions);
    std::cout << "  Done.\n";
}

// =========================================================================================
// CLI
// =========================================================================================

void print_help() {
    std::cout << "Usage: dtsort-codegen [options]\n";
    std::cout << "Options:\n";
    std::cout << "  -n <N>            Sort size (2-10) [default: generate all 2..8]\n";
    std::cout << "  --mode <mode>     Generation mode: full, gs<digits> (e.g., gs53 for 5+3 groups) [default: full]\n";
    std::cout << "  -h, --help        Show this help\n";
}

int main(int argc, char **argv) {
    if (argc > 1 && (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0)) {
        print_help();
        return 0;
    }

    std::filesystem::create_directories("output");
    std::filesystem::create_directories("include");

    int n = -1;
    std::string mode = "full";

    for(int i=1; i<argc; ++i) {
        if (strcmp(argv[i], "-n") == 0) {
            if (i+1 < argc) {
                n = std::atoi(argv[++i]);
            } else {
                std::cerr << "Error: -n requires an argument.\n";
                return 1;
            }
        } else if (strcmp(argv[i], "--mode") == 0) {
            if (i+1 < argc) {
                mode = argv[++i];
            } else {
                std::cerr << "Error: --mode requires an argument.\n";
                return 1;
            }
        } else if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
            print_help();
            return 0;
        }
    }

    if (mode != "full" && !isGsMode(mode)) {
        std::cerr << "Error: Invalid mode '" << mode << "'. Use 'full' or 'gs<digits>' (e.g., gs53).\n";
        return 1;
    }

    // Validate gs mode sum matches N
    if (isGsMode(mode) && n != -1) {
        int sum = gsGroupSum(mode);
        if (sum != n) {
            auto groups = parseGsGroups(mode);
            std::cerr << "Error: Group sizes [";
            for (size_t i = 0; i < groups.size(); ++i) {
                if (i > 0) std::cerr << ",";
                std::cerr << groups[i];
            }
            std::cerr << "] sum to " << sum << " but N=" << n << "\n";
            return 1;
        }
    }

    // For gs mode without explicit -n, derive N from group sum
    if (isGsMode(mode) && n == -1) {
        n = gsGroupSum(mode);
    }

    if (n != -1) {
         // Generate single
        switch(n) {
            case 2: generate_sorter<2>("dtsort2", mode); break;
            case 3: generate_sorter<3>("dtsort3", mode); break;
            case 4: generate_sorter<4>("dtsort4", mode); break;
            case 5: generate_sorter<5>("dtsort5", mode); break;
            case 6: generate_sorter<6>("dtsort6", mode); break;
            case 7: generate_sorter<7>("dtsort7", mode); break;
            case 8: generate_sorter<8>("dtsort8", mode); break;
            case 9: generate_sorter<9>("dtsort9", mode); break;
            case 10: generate_sorter<10>("dtsort10", mode); break;
            default: std::cerr << "N=" << n << " is out of range [2,10].\n";
        }
    } else {
        generate_sorter<2>("dtsort2", mode);
        generate_sorter<3>("dtsort3", mode);
        generate_sorter<4>("dtsort4", mode);
        generate_sorter<5>("dtsort5", mode);
        generate_sorter<6>("dtsort6", mode);
        generate_sorter<7>("dtsort7", mode);
        generate_sorter<8>("dtsort8", mode);
    }

    return 0;
}
