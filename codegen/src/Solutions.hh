#pragma once
#include "PartialMatrix.hh"
#include "PermMatrix.hh"
#include "consts.hh"
#include "debug.hh"
#include <bitset>
#include <unordered_map>
#include <vector>

struct SplitResult {
  std::vector<size_t> trueSet;
  std::vector<size_t> falseSet;
  size_t trueVariations = 0;
  size_t falseVariations = 0;
  size_t delta() const {
    return trueSet.size() > falseSet.size() ? trueSet.size() - falseSet.size()
                                            : falseSet.size() - trueSet.size();
  }
};

/***
 * Represents Solutions strategy/cache
 */
template <N_t N> class Solutions {
public:
  /** Solutions object instance */
  static Solutions &getInstance() {
    static Solutions instance;
    return instance;
  }

  /** holds all permutations (solutions) */
  std::vector<PermMatrix<N>> all;

  /**
   * How popular is each solution (permutation).
   *
   * For N elements, there are N^N possible input sequences (variations) if we
   * consider relative ordering. Many variations map to the same permutation.
   *
   * Example N=3:
   * - Input [0, 2, 1] maps to permutation (0, 2, 1).
   * - Input [1, 2, 1] maps to permutation (0, 2, 1) (stable sort semantics).
   *
   * The `counts` array stores how many of the N^N variations result in each of
   * the N! permutations. This is used to weight the decision tree splits: we
   * prefer splits that balance the *number of variations* (probability mass),
   * not just the number of permutations.
   */
  std::vector<size_t> counts = std::vector<size_t>(FACTORIAL[N], 0);

  /***
   * Evaluates a potential split by simulating a comparison question.
   *
   * @param partial The current state of knowledge (comparisons already made).
   * @param index   The index of the comparison to test (row/col in lower
   * triangle).
   * @param input   List of candidate permutation indices that are consistent
   * with `partial`.
   *
   * @return SplitResult containing:
   *         - trueSet: Permutations consistent if compare(row, col) is TRUE.
   *         - falseSet: Permutations consistent if compare(row, col) is FALSE.
   *         - trueVariations/falseVariations: Total weight (count of
   * variations) for each side.
   *         - delta: The imbalance between the two sides (used by the greedy
   * heuristic).
   */
  SplitResult split(const PartialMatrix<N> & /*partial*/, const size_t index,
                    const std::vector<size_t> &input) const {

    // Debugn("split(" << partial.str("|", true, false, index) << ", " << index
    // << "," << input << ")");

    SplitResult result;

    // We simulate setting the comparison at `index` to TRUE.
    // A candidate permutation 'matches true' if its comparison matrix has a 1
    // at `index`.
    //
    // Optimization: We iterate only through the `input` candidates (which are
    // already valid given the path to this node). We don't need to check other
    // bits of `partial` because logic guarantees they already match.

    auto &all = Solutions::getInstance().all;
    for (size_t i : input) {
      if (all[i].data[index]) { // matches true (bit is 1)
        result.trueSet.push_back(i);
        result.trueVariations += counts[i];
      } else { // must match false (bit is 0)
        result.falseSet.push_back(i);
        result.falseVariations += counts[i];
      }
    }

    // Debugn("split() returns delta=" << result.delta());
    return result;
  }

private:
  void build() {
    Debugn(" Building Solutions for N=" << (int)N << "...");
    std::unordered_map<std::bitset<N * N>, size_t> tmp;

    // Phase 1: Generate all N! permutations
    // We use Digits<N> to iterate through (0,1,2), (0,2,1), etc.
    // Each permutation creates a unique PermMatrix (lower triangle string of
    // comparisons).
    Debugn("Adding all permutations...");
    Digits<N> perm;
    perm.setFirstPermutation();
    all.reserve(FACTORIAL[N]);
    do {
      all.emplace_back(PermMatrix<N>(perm));
      tmp[all.back().data] = 1; // Mark as seen
    } while (perm.setNextPermutation());
    Debugn("Adding all permutations... Added " << all.size());

    // Phase 2: Count variations (N^N)
    // We iterate through every possible sequence of length N (values 0..N-1).
    // Each sequence maps to one of the valid permutation matrices.
    // We blindly count them to determine the "weight" of each permutation.
    if constexpr (N <= 8) {
      Debugn("Counting all variations...");
      uint64_t i = 0;

      // setVariation(i) returns false when i >= N^N
      while (perm.setVariation(i++)) {
        PermMatrix<N> pm(perm);

        // Increment the count for the matching permutation matrix.
        // Note: tmp stores map from bitset -> count.
        tmp.find(pm.data)->second++;
      }
      Debugn("Counting all variations... Processed " << --i);

      // Phase 3: Transfer counts to the flat vector
      Debugn("Building Solutions counts...");
      for (size_t i = 0; i < all.size(); ++i) {
        // Subtract 1 because we initialized map with 1 in Phase 1
        counts[i] = tmp.find(all[i].data)->second - 1;
      }
    } else {
      // For N>=9, variation counting is intractable (N^N > 387M).
      // The greedy heuristic uses permutation counts (delta), not variation
      // weights.
      std::cout << "Skipping variation counting for N=" << (int)N
                << " (N^N too large)\n";
      for (size_t i = 0; i < all.size(); ++i) {
        counts[i] = 1; // uniform weight
      }
    }
    Debugn("Building Solutions DONE");
  }

  Solutions() { build(); }

  Solutions(const Solutions &) = delete;
  Solutions &operator=(const Solutions &) = delete;
};
