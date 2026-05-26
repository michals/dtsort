"""
Runner package for dtsort benchmark orchestration.

Provides structured modules for parsing configurations, managing system environments,
building target C++ libraries, executing benchmark workflows, communicating with SQLite,
and tracking execution utilities.
"""

from runner.builder import (
    compile_benchmark_targets,
    configure_benchmark_suite,
    run_root_generation,
    run_stdlib_patching,
)
from runner.config import RunConfig, generate_log_sizes, parse_sizes
from runner.db import get_dt_sizes, ingest_data, setup_db
from runner.environment import check_auto_skip_build
from runner.executor import (
    aggregate_results,
    estimate_runtime,
    execute_stages,
    run_binary_stage,
)
from runner.utils import execute_command

__all__ = [
    "RunConfig",
    "generate_log_sizes",
    "parse_sizes",
    "execute_command",
    "check_auto_skip_build",
    "run_root_generation",
    "run_stdlib_patching",
    "configure_benchmark_suite",
    "compile_benchmark_targets",
    "run_binary_stage",
    "estimate_runtime",
    "execute_stages",
    "aggregate_results",
    "setup_db",
    "ingest_data",
    "get_dt_sizes",
]
