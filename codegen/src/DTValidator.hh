#pragma once
#include <iostream>
#include <vector>
#include <array>
#include <algorithm>
#include "DecisionTree.hh"
#include "Solutions.hh"
#include "GsMode.hh"
#include "Digits.hh"

/**
 * Validation utilities for Decision Trees.
 * These are used by tests and NOT included by core production generators.
 */

template<N_t N>
bool validateTree(const DTNode<N>& root, const Solutions<N>& solutions, size_t expectedLeaves, const std::string& mode) {
    // Check 1: Completeness - do we have exactly the expected number of leaves?
    TreeStats stats = getTreeStats(root);
    if (stats.leaves != expectedLeaves) {
        std::cerr << "Validation Error: Expected " << expectedLeaves << " leaves, found " << stats.leaves << "\n";
        return false;
    }

    // Check 2: Correctness for every candidate permutation
    for(size_t i=0; i<solutions.all.size(); ++i) {
        if (isGsMode(mode)) {
            // Skip permutations that are not group-sorted
            auto groups = parseGsGroups(mode);
            auto gsIndices = Matrix<N>::getGroupSortedIndices(groups);
            bool groupSorted = true;
            for (size_t idx : gsIndices) {
                if (solutions.all[i].data[idx]) { groupSorted = false; break; }
            }
            if (!groupSorted) continue;
        } else if (mode == "ps") {
            // Skip if not pre-sorted
            bool preSorted = true;
            auto m2Indices = Matrix<N>::getPreSortedIndices();
            for (size_t idx : m2Indices) {
                if (solutions.all[i].data[idx]) {
                    preSorted = false;
                    break;
                }
            }
            if (!preSorted) continue;
        }

        const auto& permMatrix = solutions.all[i];
        
        const DTNode<N>* curr = &root;
        while (!curr->isLeaf()) {
            bool goTrue = permMatrix.data[curr->splitIndex()];
            if (goTrue) {
                if (!curr->trueChild) {
                    std::cerr << "Validation Error: Perm " << i << " hit null true child\n";
                    return false;
                }
                curr = curr->trueChild.get();
            } else {
                if (!curr->falseChild) {
                    std::cerr << "Validation Error: Perm " << i << " hit null false child\n";
                    return false;
                }
                curr = curr->falseChild.get();
            }
        }
        
        if (curr->matchingSolutions.size() != 1) {
             std::cerr << "Validation Error: Leaf has " << curr->matchingSolutions.size() << " solutions (expected 1)\n";
             return false;
        }
        
        if (curr->matchingSolutions[0] != i) {
            std::cerr << "Validation Error: Perm " << i << " ended up at leaf for Perm " << curr->matchingSolutions[0] << "\n";
            return false;
        }
    }
    return true;
}

/**
 * Applies the cycles defined by targetPerm to the given arr.
 * This simulates the action of the leaf nodes (X macros).
 */
template<N_t N, typename T>
void applyPermutationCycles(std::array<T, N>& arr, const Digits<N>& targetPerm) {
    std::bitset<N> done;
    for (int i = 0; i < N; ++i) {
        if (done[i] || i == targetPerm.data[i]) continue;
        
        std::vector<int> cycle;
        int curr = i;
        do {
            cycle.push_back(curr);
            done.set(curr);
            curr = targetPerm.data[curr];
        } while (curr != i);
        
        // Apply right-shift cycle: arr[cycle[0]] gets arr[cycle.back()], etc.
        // X(a,b,c) means a=c, b=a, c=b.
        T tmp = arr[cycle.back()];
        for (size_t j = cycle.size() - 1; j > 0; --j) {
            arr[cycle[j]] = arr[cycle[j-1]];
        }
        arr[cycle[0]] = tmp;
    }
}

/**
 * Verifies that the decision tree actually sorts all N! permutations.
 */
template<N_t N>
bool validateSorting(const DTNode<N>& root, const Solutions<N>& solutions, const std::string& mode) {
    for (size_t i = 0; i < solutions.all.size(); ++i) {
        const auto& targetPerm = solutions.all[i].perm;
        
        std::array<int, N> arr;
        for (int n = 0; n < N; ++n) arr[n] = targetPerm.data[n];
        
        if (isGsMode(mode)) {
            // Group pre-sorting: sort each group independently
            auto groups = parseGsGroups(mode);
            int offset = 0;
            for (int g : groups) {
                std::sort(arr.begin() + offset, arr.begin() + offset + g);
                offset += g;
            }
        } else if (mode == "ps") {
            // M2: Apply pairwise pre-sorting swaps
            for (int j = 0; j + 1 < N; j += 2) {
                if (arr[j + 1] < arr[j]) std::swap(arr[j], arr[j + 1]);
            }
        }

        const DTNode<N>* curr = &root;
        while (!curr->isLeaf()) {
            if (arr[curr->row] < arr[curr->col]) {
                curr = curr->trueChild.get();
            } else {
                curr = curr->falseChild.get();
            }
        }
        
        const auto& leafPerm = solutions.all[curr->matchingSolutions[0]].perm;
        applyPermutationCycles<N, int>(arr, leafPerm);
        
        if (!std::is_sorted(arr.begin(), arr.end())) {
            std::cerr << "Sorting Validation Error: Permutation " << i << " failed to sort.\n";
            std::cerr << "  Initial: " << targetPerm << "\n";
            std::cerr << "  Result:  (";
            for(int v : arr) std::cerr << v;
            std::cerr << ")\n";
            return false;
        }
    }
    return true;
}

/**
 * Verifies that the decision tree sorts all N^N variations (sequences with duplicates).
 */
template<N_t N>
bool validateVariations(const DTNode<N>& root, const Solutions<N>& solutions, const std::string& mode) {
    Digits<N> var;
    uint64_t count = 0;
    while (var.setVariation(count++)) {
        std::array<int, N> arr;
        for (int n = 0; n < N; ++n) arr[n] = var.data[n];
        
        if (isGsMode(mode)) {
            // Group pre-sorting: sort each group independently
            auto groups = parseGsGroups(mode);
            int offset = 0;
            for (int g : groups) {
                std::sort(arr.begin() + offset, arr.begin() + offset + g);
                offset += g;
            }
        } else if (mode == "ps") {
            // M2: Apply pairwise pre-sorting swaps
            for (int j = 0; j + 1 < N; j += 2) {
                if (arr[j + 1] < arr[j]) std::swap(arr[j], arr[j + 1]);
            }
        }

        const DTNode<N>* curr = &root;
        while (!curr->isLeaf()) {
            if (arr[curr->row] < arr[curr->col]) {
                curr = curr->trueChild.get();
            } else {
                curr = curr->falseChild.get();
            }
        }
        
        const auto& leafPerm = solutions.all[curr->matchingSolutions[0]].perm;
        applyPermutationCycles<N, int>(arr, leafPerm);
        
        if (!std::is_sorted(arr.begin(), arr.end())) {
            std::cerr << "Variation Validation Error: Variation " << (count-1) << " failed to sort.\n";
            std::cerr << "  Initial: " << var << "\n";
            std::cerr << "  Result:  (";
            for(int v : arr) std::cerr << v;
            std::cerr << ")\n";
            return false;
        }
    }
    return true;
}
struct ExhaustiveMetrics {
    double avgCmp;
    size_t maxCmp;
    double avgMove;
    size_t maxMove;
};

/**
 * Calculates exhaustive performance metrics across all N! permutations.
 */
template<N_t N>
ExhaustiveMetrics getExhaustiveMetrics(const DTNode<N>& root, const Solutions<N>& solutions, const std::string& mode) {
    uint64_t totalCmp = 0;
    size_t maxCmp = 0;
    uint64_t totalMove = 0;
    size_t maxMove = 0;
    
    // We iterate through ALL N! permutations to get true average/max
    Digits<N> perm;
    perm.setFirstPermutation();
    do {
        std::array<int, N> arr;
        for (int i = 0; i < N; ++i) arr[i] = (int)perm.data[i];
        
        size_t currentCmp = 0;
        size_t currentMove = 0;
        
        if (isGsMode(mode)) {
            // Group pre-sorting comparisons and moves
            // Use OEIS A036604 for comparison count per group
            // Simulate actual sorting for move counting
            auto groups = parseGsGroups(mode);
            int offset = 0;
            for (int g : groups) {
                if (g > 1) {
                    // Count comparisons as optimal for this group size
                    // (We use the OEIS bound — the actual dtsortK achieves this)
                    extern const int OEIS_A036604[];
                    currentCmp += OEIS_A036604[g];
                    // Simulate sorting for move counting
                    std::array<int, N> before;
                    std::copy(arr.begin() + offset, arr.begin() + offset + g, before.begin());
                    std::sort(arr.begin() + offset, arr.begin() + offset + g);
                    // Count how many positions changed (conservative move estimate)
                    for (int j = 0; j < g; ++j) {
                        if (before[j] != arr[offset + j]) currentMove++;
                    }
                }
                offset += g;
            }
        } else if (mode == "ps") {
            // Apply pairwise pre-sorting swaps (CAS)
            for (int j = 0; j + 1 < N; j += 2) {
                currentCmp++;
                if (arr[j + 1] < arr[j]) {
                    std::swap(arr[j], arr[j + 1]);
                    currentMove += 3; // std::swap is 3 moves
                }
            }
        }
        
        // Traverse decision tree
        const DTNode<N>* curr = &root;
        while (!curr->isLeaf()) {
            currentCmp++;
            if (arr[curr->row] < arr[curr->col]) {
                curr = curr->trueChild.get();
            } else {
                curr = curr->falseChild.get();
            }
        }
        
        // Apply cycles at leaf
        size_t leafPermIdx = curr->matchingSolutions[0];
        const auto& leafPerm = solutions.all[leafPermIdx].perm;
        currentMove += leafPerm.countMoves();
        
        totalCmp += currentCmp;
        if (currentCmp > maxCmp) maxCmp = currentCmp;
        
        totalMove += currentMove;
        if (currentMove > maxMove) maxMove = currentMove;
        
    } while (perm.setNextPermutation());
    
    ExhaustiveMetrics m;
    m.avgCmp = (double)totalCmp / FACTORIAL[N];
    m.maxCmp = maxCmp;
    m.avgMove = (double)totalMove / FACTORIAL[N];
    m.maxMove = maxMove;
    return m;
}
