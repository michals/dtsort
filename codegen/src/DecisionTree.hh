#pragma once
#include "Matrix.hh"
#include "PartialMatrix.hh"
#include "Solutions.hh"
#include <algorithm>
#include <memory>
#include <vector>

template <N_t N> struct DTNode {
  N_t row, col; // which comparison to ask (row > col, lower triangle)
  std::vector<size_t> matchingSolutions; // indices into Solutions::all
  size_t variationCount; // sum of counts[] for matching solutions

  // Children (nullptr for leaf nodes)
  std::unique_ptr<DTNode> trueChild;
  std::unique_ptr<DTNode> falseChild;

  // A leaf is a node that identifies exactly ONE permutation.
  // If matchingSolutions.size() == 0 it's an error/impossible state.
  bool isLeaf() const { return matchingSolutions.size() == 1; }

  // Computed split index for Matrix operations
  size_t splitIndex() const { return Matrix<N>::rc2i(row, col); }

  size_t depth() const {
    if (isLeaf())
      return 0;
    size_t d0 = trueChild ? trueChild->depth() : 0;
    size_t d1 = falseChild ? falseChild->depth() : 0;
    return 1 + std::max(d0, d1);
  }

  size_t countNodes() const {
    size_t sum = 1;
    if (trueChild)
      sum += trueChild->countNodes();
    if (falseChild)
      sum += falseChild->countNodes();
    return sum;
  }
};

template <N_t N>
std::unique_ptr<DTNode<N>>
buildTree(PartialMatrix<N> partial,
          const std::vector<size_t> &candidateSolutions,
          const Solutions<N> &solutions) {
  auto node = std::unique_ptr<DTNode<N>>(new DTNode<N>());
  node->matchingSolutions = candidateSolutions;

  // Calc total variation count
  node->variationCount = 0;
  for (size_t idx : candidateSolutions) {
    node->variationCount += solutions.counts[idx];
  }

  // Base case: leaf node
  // If we have 1 candidate, we are done.
  // If we have 0, something went wrong (should be impossible if logic is
  // correct). If we have >1 but no unknown cells left (impossible to
  // distinguish), also stop.
  auto unknowns = partial.unknownYet();
  if (candidateSolutions.size() <= 1 || unknowns.empty()) {
    return node;
  }

  // Find best split
  // "Best" is defined by the greedy heuristic: minimize delta = |trueSet -
  // falseSet| This aims to create a balanced tree (depth ~ log2(N!)).

  size_t bestIndex = 0;
  size_t minDelta = -1; // max size_t
  // size_t bestVariationSum = 0; // Unused

  // Cache the best split result so we don't have to re-compute it for the
  // chosen index
  SplitResult bestResult;
  bool foundSplit = false;

  // Try every unknown comparison (valid candidate splits)
  for (size_t index : unknowns) {
    SplitResult result = solutions.split(partial, index, candidateSolutions);

    // If one side is empty, this index gives no information for this set of
    // candidates. This implies the outcome is already determined by previous
    // comparisons (transitivity). e.g. if a<b and b<c, then asking a<c will
    // always be true. We skip these "implied" comparisons to prune the tree.
    if (result.trueSet.empty() || result.falseSet.empty()) {
      continue;
    }

    size_t delta = result.delta();

    // Greedy heuristic: minimize delta (balance the tree).
    // A delta of 0 means a perfect 50/50 split of candidates.
    //
    // NOTE: This greedy approach is equivalent to maximizing Information Gain
    // (ID3/C4.5) because we are splitting a set of equiprobable items.
    // Minimizing |S_true - S_false| <==> Maximizing Entropy.

    if (delta < minDelta) {
      minDelta = delta;
      bestIndex = index;
      bestResult = result;
      foundSplit = true;

    } else if (delta == minDelta) {
      // Tie-breaking: currently just picks the first one found.
      // A better tie-breaker might look at variation counts or lookahead,
      // but for N=5 this is sufficient to find optimal trees.
    }
  }

  // If no useful split found (all unknowns imply one outcome or another),
  // we effectively have a "leaf" that matches multiple solutions.
  //
  // In a correct sorting network for specific N, this shouldn't happen if we
  // strictly follow the information limit, but for checking connectivity it
  // might. However, if we can't split, we return what we have (often a leaf
  // with multiple candidates).
  if (!foundSplit) {
    return node;
  }

  // Apply best split
  node->row = Matrix<N>::i2r(bestIndex);
  node->col = Matrix<N>::i2c(bestIndex);

  // Recurse Left (True)
  partial.set(bestIndex, true);
  node->trueChild = buildTree(partial, bestResult.trueSet, solutions);

  // Recurse Right (False)
  partial.set(bestIndex, false); // backtrack/set for next branch
  node->falseChild = buildTree(partial, bestResult.falseSet, solutions);

  return node;
}

struct TreeStats {
  size_t nodes = 0;
  size_t leaves = 0;
  size_t maxDepth = 0;
  size_t minDepth = -1;
  size_t totalPathLength = 0;

  double avgDepth() const {
    return leaves > 0 ? (double)totalPathLength / leaves : 0.0;
  }
};

template <N_t N>
void computeStats(const DTNode<N> &node, TreeStats &stats,
                  size_t currentDepth) {
  stats.nodes++;

  if (node.isLeaf()) {
    stats.leaves++;
    if (currentDepth > stats.maxDepth)
      stats.maxDepth = currentDepth;
    if (currentDepth < stats.minDepth)
      stats.minDepth = currentDepth;
    stats.totalPathLength += currentDepth;
    return;
  }

  if (node.trueChild)
    computeStats(*node.trueChild, stats, currentDepth + 1);
  if (node.falseChild)
    computeStats(*node.falseChild, stats, currentDepth + 1);
}

template <N_t N> TreeStats getTreeStats(const DTNode<N> &root) {
  TreeStats stats;
  computeStats(root, stats, 0);
  return stats;
}

/**
 * @brief Calculate the path length for a specific permutation through the tree.
 * This counts how many comparisons are needed to reach the leaf for a given
 * input.
 */
template <N_t N>
int getPathLength(const DTNode<N> &root, const Digits<N> &perm) {
  int len = 0;
  const DTNode<N> *curr = &root;
  while (!curr->isLeaf()) {
    len++;
    if (perm.data[curr->row] < perm.data[curr->col]) {
      curr = curr->trueChild.get();
    } else {
      curr = curr->falseChild.get();
    }
  }
  return len;
}
