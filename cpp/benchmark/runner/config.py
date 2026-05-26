"""
Orchestration and CLI configuration parser.

Encapsulates benchmark configurations, supported compilers, types, input
distributions, and geometric N sizing progressions with thorough docstring coverage.
"""

import argparse
import math
from dataclasses import dataclass, field

from runner.consts import SUPPORTED_TYPES, BenchmarkImpl, BenchmarkStage, Variant


def get_variants(impl: BenchmarkImpl, adaptivity: str) -> list[Variant]:
    """
    Resolves the list of binary variants to build/run for a given implementation.

    For standard library implementations (GCC, LLVM), produces ["stock", "dtsort"].
    For the standalone dt_stable_sort engine, produces ["stock"] plus any requested
    adaptivity variants ("dtsort-em" for entropy, "dtsort-am" for adaptive).

    Args:
        impl: The implementation identifier (e.g. BenchmarkImpl.GCC14).
        adaptivity: Comma-separated adaptivity modes (e.g. "entropy,adaptive").

    Returns:
        A list of variant suffixes for the given implementation.
    """
    if impl == BenchmarkImpl.DT_STABLE_SORT:
        variants = [Variant.STOCK]
        if "entropy" in adaptivity:
            variants.append(Variant.DTSORT_EM)
        if "adaptive" in adaptivity:
            variants.append(Variant.DTSORT_AM)
        return variants
    return [Variant.STOCK, Variant.DTSORT]


def _generate_log_sizes(count: int, min_val: int, max_val: int) -> list[int]:
    """
    Generates a list of geometrically (logarithmically) spaced dataset sizes in
    the range [min_val, max_val] to evaluate performance scaling characteristics.

    Duplicates from rounding at lower boundaries are dynamically filled to guarantee
    exactly 'count' distinct sizes if possible.

    Args:
        count: The number of distinct sizes desired.
        min_val: The minimum size bound (e.g. 20).
        max_val: The maximum size bound (e.g. 2,000,000).

    Returns:
        A sorted list of geometrically spaced integer sizes.
    """
    if count <= 1:
        return [min_val]

    sizes = set()
    log_min = math.log(min_val)
    log_max = math.log(max_val)

    # Generate points geometrically spaced
    for i in range(count):
        t = i / (count - 1)
        val = round(math.exp(log_min + t * (log_max - log_min)))
        sizes.add(val)

    sorted_sizes = sorted(list(sizes))

    # If some values duplicated at the lower end due to rounding,
    # fill them in to reach the target count
    attempts = 0
    while len(sorted_sizes) < count and attempts < 1000:
        attempts += 1
        # Find the largest gap in log space and insert a point
        best_gap = -1.0
        insert_idx = -1
        insert_val = -1

        for i in range(len(sorted_sizes) - 1):
            s1, s2 = sorted_sizes[i], sorted_sizes[i + 1]
            if s2 - s1 > 1:
                log_gap = math.log(s2) - math.log(s1)
                if log_gap > best_gap:
                    best_gap = log_gap
                    insert_idx = i + 1
                    insert_val = round(math.exp((math.log(s1) + math.log(s2)) / 2))

        if insert_val != -1 and insert_val not in sorted_sizes:
            sorted_sizes.insert(insert_idx, insert_val)
        else:
            break

    return sorted_sizes[:count]


def generate_log_sizes(count: int, min_val: int, max_val: int) -> list[int]:
    """Public wrapper for geometric size list generation."""
    return _generate_log_sizes(count, min_val, max_val)


def parse_sizes(sizes_str: str | list[int]) -> list[int]:
    """
    Parses a sizes argument string, resolving it into a list of integer sizes.

    Supports either explicit comma-separated values (e.g. "50,100,200") or a
    geometric range descriptor (e.g. "100:20:2000000").

    Args:
        sizes_str: The raw argument string from CLI or a list of ints.

    Returns:
        A list of integer sizes.

    Raises:
        argparse.ArgumentTypeError: If the range parameters cannot be parsed or are invalid.
    """
    if isinstance(sizes_str, list):
        return sizes_str
    if not sizes_str:
        return []
    sizes_str = sizes_str.strip()
    if ":" in sizes_str:
        parts = [p.strip() for p in sizes_str.split(":")]
        if len(parts) == 3:
            try:
                count = int(parts[0])
                min_val = int(parts[1])
                max_val = int(parts[2])
                return _generate_log_sizes(count, min_val, max_val)
            except ValueError:
                raise argparse.ArgumentTypeError("Sizes in count:min:max format must be integers.")
        else:
            raise argparse.ArgumentTypeError(
                "Sizes in count:min:max format must have exactly 3 parts separated by colons."
            )
    try:
        return [int(p.strip()) for p in sizes_str.split(",") if p.strip()]
    except ValueError:
        raise argparse.ArgumentTypeError("Explicit sizes must be comma-separated integers.")


@dataclass
class RunConfig:
    """
    Configuration parameters mapping the entire benchmark execution lifecycle.

    Contains statistical iteration counts, target implementations, datatypes,
    size arrays, and adaptivity modes.
    """

    max_n: int = 10
    count_iters: int = 20
    bench_iters: int = 7
    skip_build: bool = False
    mode: str = "gs"
    impls: list[BenchmarkImpl] = field(default_factory=lambda: [BenchmarkImpl.DT_STABLE_SORT])
    types: str = "IntNaked,WideRow"
    sizes: list[int] = field(default_factory=list)
    sizes_explicit: bool = False
    only_counts: bool = False
    stages: list[BenchmarkStage] = field(default_factory=list)
    inputs: str = "random,reversed"
    adaptivity: str = "adaptive"
    dry_run: bool = False

    def __post_init__(self) -> None:
        # In case we pass a string representation or None, parse it automatically
        if isinstance(self.sizes, str) or not self.sizes:
            self.sizes = parse_sizes(self.sizes)
        if not self.stages:
            if self.only_counts:
                self.stages = [BenchmarkStage.VALIDATE, BenchmarkStage.COUNTS, BenchmarkStage.CONSISTENCY]
            else:
                self.stages = list(BenchmarkStage)


def parse_cli_args(args_list: list[str] | None = None) -> RunConfig:
    """
    Parses CLI parameters from a list of arguments, constructing a RunConfig instance.

    Args:
        args_list: List of arguments to parse (defaults to sys.argv[1:]).

    Returns:
        An instantiated RunConfig.
    """
    parser = argparse.ArgumentParser(
        description="dtsort Modernized Benchmark Suite Orchestrator",
        formatter_class=argparse.ArgumentDefaultsHelpFormatter,
    )
    parser.add_argument(
        "--count-iters", "-c", type=int, default=20, help="Number of random iterations for counting operations"
    )
    parser.add_argument(
        "--bench-iters", "-b", type=int, default=7, help="Statistical repetitions for wall-clock benchmarks"
    )
    parser.add_argument("--skip-build", "-s", action="store_true", help="Skip regeneration and cmake/make rebuild")
    parser.add_argument("--only-counts", action="store_true", help="Skip time-consuming wall-clock benchmarks")
    parser.add_argument(
        "--stages",
        type=str,
        default="all",
        help=f"Comma-separated stages to execute. Supported: {', '.join(s.value for s in BenchmarkStage)} or 'all'",
    )
    parser.add_argument(
        "--impls", default="DT", help="Comma-separated list of implementations (GCC14, GCC16, GCC7, LLVM15, LLVM19, DT)"
    )
    parser.add_argument(
        "--sizes", type=str, default="100:20:2000000", help="Override large N sizes (comma separated or count:min:max)"
    )
    parser.add_argument(
        "--types",
        type=str,
        default="IntNaked,WideRow",
        help=f"Comma-separated data types to benchmark. Supported: {', '.join(SUPPORTED_TYPES)}",
    )
    parser.add_argument("--max-n", "-n", type=int, default=10, help="Max dtsort chunk size to benchmark")
    parser.add_argument(
        "--inputs",
        type=str,
        default="random,reversed",
        help="Comma-separated input distributions. Supported: random, reversed, sorted",
    )
    parser.add_argument(
        "--adaptivity", type=str, default="adaptive", help="Comma-separated adaptivity variants (e.g. entropy,adaptive)"
    )
    parser.add_argument(
        "--dry-run",
        action="store_true",
        help="Estimate and display the total benchmark run time without executing actual benchmarks",
    )

    args = parser.parse_args(args_list)

    # Process implementation list, supporting 'DT' as alias for 'dt_stable_sort'
    requested_impls = args.impls.split(",") if args.impls else ["GCC14"]
    impl_list = []
    for impl in requested_impls:
        stripped = impl.strip()
        if stripped == "DT":
            impl_list.append(BenchmarkImpl.DT_STABLE_SORT)
        else:
            try:
                impl_list.append(BenchmarkImpl(stripped))
            except ValueError:
                valid_names = ", ".join(x.value for x in BenchmarkImpl)
                raise argparse.ArgumentTypeError(
                    f"Invalid implementation '{stripped}'. Supported values are: {valid_names} (or 'DT' as alias)"
                ) from None

    # Resolve stages
    if args.stages == "all":
        if args.only_counts:
            stages_list = [BenchmarkStage.VALIDATE, BenchmarkStage.COUNTS, BenchmarkStage.CONSISTENCY]
        else:
            stages_list = list(BenchmarkStage)
    else:
        stages_list = []
        for s in args.stages.split(","):
            stripped = s.strip()
            if not stripped:
                continue
            try:
                stages_list.append(BenchmarkStage(stripped))
            except ValueError:
                valid_names = ", ".join(x.value for x in BenchmarkStage)
                raise argparse.ArgumentTypeError(
                    f"Invalid stage '{stripped}'. Supported values are: {valid_names} (or 'all')"
                ) from None

    only_counts_resolved = args.only_counts or not any(
        s in stages_list
        for s in (BenchmarkStage.WEIGHT, BenchmarkStage.BENCHMARK_SMALL, BenchmarkStage.BENCHMARK_LARGE)
    )

    is_sizes_explicit = args.sizes != "100:20:2000000"
    sizes_expanded = parse_sizes(args.sizes)

    return RunConfig(
        max_n=args.max_n,
        count_iters=args.count_iters,
        bench_iters=args.bench_iters,
        skip_build=args.skip_build,
        mode="gs",
        impls=impl_list,
        types=args.types,
        sizes=sizes_expanded,
        sizes_explicit=is_sizes_explicit,
        only_counts=only_counts_resolved,
        stages=stages_list,
        inputs=args.inputs,
        adaptivity=args.adaptivity,
        dry_run=args.dry_run,
    )
