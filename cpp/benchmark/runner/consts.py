"""
Central simple constants module for project-wide C++ sorting benchmark orchestrator
and reporting tools.
"""

from enum import Enum


class BenchmarkImpl(str, Enum):
    """Supported C++ compiler/standard library implementations under test."""

    GCC7 = "GCC7"
    GCC14 = "GCC14"
    GCC16 = "GCC16"
    LLVM15 = "LLVM15"
    LLVM19 = "LLVM19"
    DT_STABLE_SORT = "dt_stable_sort"

    def __str__(self) -> str:
        return str(self.value)


class DataType(str, Enum):
    """Supported target C++ datatypes to benchmark."""

    INT_NAKED = "IntNaked"
    MULTI_KEY = "MultiKey"
    WIDE_ROW = "WideRow"
    LONG_PREFIX = "LongPrefix"
    REV_DOMAIN = "RevDomain"
    CASE_INSENS = "CaseInsens"

    def __str__(self) -> str:
        return str(self.value)


class Variant(str, Enum):
    """Supported binary variants constructed during stdlib patching."""

    STOCK = "stock"
    DTSORT = "dtsort"
    DTSORT_EM = "dtsort-em"
    DTSORT_AM = "dtsort-am"

    def __str__(self) -> str:
        return str(self.value)


class BenchmarkStage(str, Enum):
    """Supported pipeline execution stages."""

    VALIDATE = "validate"
    COUNTS = "counts"
    CONSISTENCY = "consistency"
    WEIGHT = "weight"
    BENCHMARK_SMALL = "benchmark_small"
    BENCHMARK_LARGE = "benchmark_large"

    def __str__(self) -> str:
        return str(self.value)


# Supported compilers/implementations under test (as standard strings)
SUPPORTED_IMPLS = [impl.value for impl in BenchmarkImpl]

# Supported target C++ datatypes to benchmark (as standard strings)
SUPPORTED_TYPES = [t.value for t in DataType]

# Default large size ranges to evaluate in count:min:max spacing format
DEFAULT_LARGE_SIZES = "100:20:2000000"

# Information theoretic lower bound of comparisons for small N arrays (OEIS A036604)
# Maps N -> optimal worst-case comparisons
OEIS_A036604 = {0: 0, 1: 0, 2: 1, 3: 3, 4: 5, 5: 7, 6: 10, 7: 13, 8: 16, 9: 19, 10: 22}
