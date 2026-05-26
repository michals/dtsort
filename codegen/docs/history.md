# The Story of dtsort: A 20-Year Journey

The `dtsort` project represents a nearly two-decade intermittent journey driven by curiosity to find the ultimate limits of comparison-based sorting.

## The Holy Grail of Sorting

The story begins during university computer science studies. After learning that comparison-based sorting has an asymptotic lower bound of $\mathcal{O}(N \log N)$ and studying standard algorithms like MergeSort and QuickSort, a realization emerged: there is a significant gap between asymptotic $\mathcal{O}(N \log N)$ efficiency and absolute optimality. Asymptotic bounds hide constant factors, leaving the question of what an absolutely perfect sorting algorithm would look like in terms of raw, non-asymptotic counts.

The goal was defined as finding a "perfect sort": an algorithm that achieves the absolute mathematical minimum number of comparisons, the absolute minimum number of element swaps (moves), while simultaneously operating strictly in-place and maintaining stability. These are exceptionally bold requirements for a single comparison-sorting algorithm, as practical implementations often trade stability for in-place execution or vice versa.

## Starting Small: hgsort2 to hgsort4

Rather than attempting to solve the general problem immediately, the approach shifted to the smallest possible cases.

- For $N=2$, the solution is trivial: 1 comparison, up to 1 swap. This was dubbed `hgsort2()` ("Holy Grail Sort").
- For $N=3$, a software engineer could quickly devise a solution: 2 initial comparisons, followed by potentially a 3rd comparison to gain full information, placing elements in their exact positions. This yielded `hgsort3()`.
- For $N=4$, after a few minutes with pencil and paper, `hgsort4()` was manually crafted.

However, $N=5$ proved too complex to construct by hand. Yet, a clear structural pattern had emerged. Instead of manually writing the sorting logic, the focus shifted to writing an algorithm that could analyze this structure and generate the `hgsortX()` function for any given $N$. At this point, the project was paused for several years due to other student responsibilities.

## The Code Generator (Pre-AI Era)

Several years later, during a holiday season, an algorithmic puzzle sparked a return to the project. This was long before modern AI coding assistants existed; the research had to be done entirely manually.

Key theoretical breakthroughs occurred during this phase:

1. **Decision Trees:** Realizing that the algorithm required building a well-balanced binary decision tree where each leaf node perfectly identifies one of the $N!$ possible permutations.
2. **Cycle Decomposition:** Discovering that once a permutation is identified, the elements can be sorted in-place optimally by applying the inverse permutation using disjoint cycles (a mathematical concept rediscovered independently for this purpose).
3. **Greedy ID3 Tree Building:** Finding a greedy heuristic to construct the decision tree by minimizing entropy at each split (a technique later recognized as equivalent to ID3).
4. **Comparison Matrices:** Inventing a highly efficient bit-dense representation of comparison outcomes that could be packed into a `uint64` for $N \le 9$, making the tree generation computationally feasible.

The code generator was successfully implemented. It validated that `hgsort2()` through `hgsort7()` met the bold requirements for perfect sorting. `hgsort8()` worked but required a maximum of 17 comparisons instead of the theoretically optimal 16.

However, a harsh reality emerged: the generated code size grew factorially. A perfectly balanced tree for $N=8$ requires $8! = 40,320$ leaf nodes. The resulting C++ header was massive, and the compiler struggled to handle it. The universal perfect sort seemed impractical for $N > 8$.

## Making it Practical: The Standard Library Integration

If the algorithm was impractical for large arrays, could it still be useful? Real-world standard library `std::sort` and `std::stable_sort` implementations are typically hybrid algorithms (like Timsort or Introsort) that fall back to simpler sorts like Insertion Sort for small subarrays at the bottom of their recursion trees.

The new goal: hook the extremely optimal, generated small-sort functions into the bottom of standard library sorts.

Using GCC 7's `std::stable_sort()`, the complex internal source code was manually analyzed and patched. The effort succeeded, showing that integrating the decision-tree logic made the stock standard library slightly faster. To measure this accurately, Google Benchmark was introduced, utilizing custom hooks on `operator<` and move constructors to count exact comparisons and swaps, alongside wall-clock times.

The holiday season ended, and the project was paused for another couple of years. The raw proof-of-concept from this era was published to GitHub and remains available as a historical artifact: [michals/dtsort-legacy](https://github.com/michals/dtsort-legacy/).

## 2026: The Final Polish and 'dtsort'

In 2026, the project was revived. `hgsort` was officially renamed to `dtsort` to reflect its core mechanism: Decision Trees.

The infrastructure was vastly expanded:

- **Broad Toolchain Support:** Integration was added for modern GCC (14, 16) and LLVM (15, 19).
- **SQL Benchmarking Pipeline:** A robust, database-driven benchmarking suite was built to transparently track and analyze metrics.
- **Architectural Breakthroughs:** Several massive optimizations were developed and evaluated:
  - Cache-friendly top-to-bottom (DFS) recursion instead of the default BFS approach.
  - "Block-Scoped Index Sorting" to decouple comparison logic from heavy element moves.
  - Block size alignment (discovering the 128-element sweet spot for Apple M-series L2 cache).
  - **Pair-Sorting Decision Tree (psDT) Compression**: An initial breakthrough that pre-sorted pairs of elements, reducing the $N=8$ decision tree size by 16x and unexpectedly hitting the optimal 16 comparisons for $N=8$.
  - **Group-Sorted Decision Tree (gsDT) Architecture**: Shortly after psDT, the concept was generalized to arbitrary group sizes. By sorting larger independent groups (e.g., two groups of 5 for $N=10$) first, the decision tree is no longer used to "sort from scratch"—instead, it becomes an optimal, branchless **multi-sorted groups merge algorithm** that interleaves the pre-sorted sequences. This breakthrough completely eliminated the factorial code bloat problem, compressing the theoretically gigabytes-large $N=10$ tree into a highly cache-resident suite of just ~56 KB of generated C++ source code, running at a blazing ~11 nanoseconds per sort.
  - **Adaptive Mode (`DT_ADAPTIVE`)**: Rather than running a costly, separate "prescan" run to detect sorted sequences (which would add measurable overhead on random data), the generator embeds adaptivity directly into the decision tree state machine. Fully unrolled base kernels ($N \le 5$) return a boolean flag indicating if elements were already sorted. Group-sorted kernels ($N \ge 6$) leverage this by verifying the sorted flags of their pre-sorted sub-groups (e.g., two groups of 5 for $N=10$) and performing a single boundary comparison. If both are sorted and the boundary is ordered, the entire gsDT merge phase is bypassed. Combined with recursive merge-skipping at the top-level `dt_stable_sort` orchestrator, this yields an adaptivity effect similar to $\mathcal{O}(N)$ in practice for sorted or partially sorted inputs while keeping runtime overhead almost zero on high-entropy data.

What started as a student's curiosity about "perfect sorting" evolved into a comprehensive code generation engine capable of pushing the absolute theoretical limits of comparison-based sorting and demonstrating real-world potential within modern C++ standard libraries.
