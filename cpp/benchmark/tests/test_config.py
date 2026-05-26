from runner.config import RunConfig, parse_cli_args
from runner.consts import BenchmarkImpl, BenchmarkStage


def test_run_config_defaults():
    config = RunConfig()
    assert config.max_n == 10
    assert config.count_iters == 20
    assert config.bench_iters == 7
    assert config.skip_build is False
    assert config.mode == "gs"
    # Note: defaults config.impls to ["dt_stable_sort"]
    assert config.impls == [BenchmarkImpl.DT_STABLE_SORT]
    assert config.only_counts is False
    assert config.dry_run is False


def test_run_config_custom_overrides():
    config = RunConfig(
        max_n=8,
        count_iters=10,
        bench_iters=5,
        skip_build=True,
        mode="gsDT",
        impls=[BenchmarkImpl.LLVM15, BenchmarkImpl.DT_STABLE_SORT],
        types="IntNaked",
        sizes="10,20",
        sizes_explicit=True,
        only_counts=True,
        inputs="random,sorted",
        adaptivity="entropy",
        dry_run=True,
    )
    assert config.max_n == 8
    assert config.count_iters == 10
    assert config.bench_iters == 5
    assert config.skip_build is True
    assert config.mode == "gsDT"
    assert config.impls == [BenchmarkImpl.LLVM15, BenchmarkImpl.DT_STABLE_SORT]
    assert config.types == "IntNaked"
    assert config.sizes == [10, 20]
    assert config.sizes_explicit is True
    assert config.only_counts is True
    assert config.inputs == "random,sorted"
    assert config.adaptivity == "entropy"
    assert config.dry_run is True


def test_run_config_stages_override():

    # 1. Parsing single stage
    config = parse_cli_args(["--stages", "counts"])
    assert config.stages == [BenchmarkStage.COUNTS]
    assert config.only_counts is True  # No benchmark timing stages present

    # 2. Parsing multiple stages
    config = parse_cli_args(["--stages", "counts,consistency,validate"])
    assert set(config.stages) == {BenchmarkStage.COUNTS, BenchmarkStage.CONSISTENCY, BenchmarkStage.VALIDATE}
    assert config.only_counts is True

    # 3. Parsing stages including benchmarking
    config = parse_cli_args(["--stages", "counts,benchmark_small"])
    assert set(config.stages) == {BenchmarkStage.COUNTS, BenchmarkStage.BENCHMARK_SMALL}
    assert config.only_counts is False  # benchmark_small timing stage present
