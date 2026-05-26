"""
Unit and integration tests for the modernized, modular benchmark orchestrator package.

Asserts correct behavior for runner/ config parser, environment skip checks,
timing estimations, SQLite table/view creations, execution utilities, and aggregation helpers.
"""

import argparse
import subprocess
from pathlib import Path
from unittest.mock import MagicMock, patch

import pytest

from runner.config import RunConfig, generate_log_sizes, parse_cli_args, parse_sizes
from runner.db import setup_db
from runner.environment import check_auto_skip_build
from runner.executor import (
    estimate_runtime,
)
from runner.utils import execute_command


def test_modular_run_config_defaults() -> None:
    """
    Asserts default configuration parameters in RunConfig.
    """
    config = RunConfig()
    assert config.max_n == 10
    assert config.count_iters == 20
    assert config.bench_iters == 7
    assert config.skip_build is False
    assert config.mode == "gs"
    assert config.impls == ["dt_stable_sort"]
    assert config.only_counts is False
    assert config.dry_run is False


def test_modular_run_config_overrides() -> None:
    """
    Asserts custom parameters in RunConfig are preserved.
    """
    config = RunConfig(
        max_n=6,
        count_iters=50,
        bench_iters=10,
        skip_build=True,
        mode="gsDT",
        impls=["LLVM19"],
        types="WideRow",
        sizes="50,100",
        sizes_explicit=True,
        only_counts=True,
        inputs="reversed",
        adaptivity="entropy",
        dry_run=True,
    )
    assert config.max_n == 6
    assert config.count_iters == 50
    assert config.bench_iters == 10
    assert config.skip_build is True
    assert config.mode == "gsDT"
    assert config.impls == ["LLVM19"]
    assert config.types == "WideRow"
    assert config.sizes == [50, 100]
    assert config.sizes_explicit is True
    assert config.only_counts is True
    assert config.inputs == "reversed"
    assert config.adaptivity == "entropy"
    assert config.dry_run is True


def test_modular_parse_cli_args() -> None:
    """
    Asserts CLI parser maps string variables correctly.
    """
    config = parse_cli_args(["-n", "8", "-c", "100", "-b", "5", "--skip-build", "--only-counts"])
    assert config.max_n == 8
    assert config.count_iters == 100
    assert config.bench_iters == 5
    assert config.skip_build is True
    assert config.only_counts is True


def test_modular_generate_log_sizes() -> None:
    """
    Asserts the geometric log spacing size generator.
    """
    sizes = generate_log_sizes(5, 20, 2000)
    assert len(sizes) == 5
    assert sizes[0] == 20
    assert sizes[-1] == 2000
    assert sizes == sorted(sizes)


def test_modular_parse_sizes_explicit() -> None:
    """
    Asserts raw comma-separated lists parse cleanly.
    """
    assert parse_sizes("50, 100, 200") == [50, 100, 200]


def test_modular_parse_sizes_range() -> None:
    """
    Asserts count:min:max range splits geometrically.
    """
    parsed = parse_sizes("5:10:1000")
    assert len(parsed) == 5
    assert parsed[0] == 10
    assert parsed[-1] == 1000


def test_modular_parse_sizes_invalid() -> None:
    """
    Asserts parser raises ArgumentTypeError on bad formats.
    """
    with pytest.raises(argparse.ArgumentTypeError):
        parse_sizes("5:10")
    with pytest.raises(argparse.ArgumentTypeError):
        parse_sizes("abc:10:100")


def test_modular_utils_execute_command_success(tmp_path: Path) -> None:
    """
    Asserts execute_command returns zero on successful executions.
    """
    log_file = tmp_path / "test.log"
    code, out = execute_command(["echo", "hello"], cwd=tmp_path, log_file=log_file, capture=True)
    assert code == 0
    assert "hello" in out.lower()
    assert log_file.exists()
    assert "hello" in log_file.read_text(encoding="utf-8").lower()


def test_modular_utils_execute_command_failure(tmp_path: Path) -> None:
    """
    Asserts execute_command raises CalledProcessError on error status code.
    """
    with pytest.raises(subprocess.CalledProcessError):
        execute_command(["false"], cwd=tmp_path)


def test_modular_db_setup_views(tmp_path: Path) -> None:
    """
    Asserts setup_db creates SQLite tables and reporting views correctly.
    """
    db_file = tmp_path / "test.db"
    conn = setup_db(db_file)
    c = conn.cursor()

    # Verify tables
    c.execute("SELECT name FROM sqlite_master WHERE type='table'")
    tables = [r[0] for r in c.fetchall()]
    assert "run_info" in tables
    assert "consistency" in tables
    assert "large_counts" in tables
    assert "wall_clock" in tables
    assert "type_weights" in tables
    assert "code_size" in tables

    # Verify views
    c.execute("SELECT name FROM sqlite_master WHERE type='view'")
    views = [r[0] for r in c.fetchall()]
    assert "v_consistency_summary" in views
    assert "v_large_n_summary" in views
    assert "v_wall_clock_summary" in views
    assert "v_weight_summary" in views

    conn.close()


def test_modular_environment_check_skip_false(tmp_path: Path) -> None:
    """
    Asserts check_auto_skip_build returns False if CMakeCache is missing.
    """
    config = RunConfig()
    build_dir = tmp_path / "build"
    root_dir = tmp_path / "root"
    assert check_auto_skip_build(config, build_dir, root_dir) is False


def test_modular_executor_estimate_runtime(tmp_path: Path) -> None:
    """
    Asserts estimate_runtime returns realistic ETA and test counts.
    """
    config = RunConfig(dry_run=True, skip_build=True, only_counts=True)
    build_dir = tmp_path / "build"

    with patch("subprocess.run") as mock_run:
        mock_run.return_value = MagicMock(returncode=0, stdout="test_case_1\n")
        unique_tests, total_runs, est_seconds = estimate_runtime(config, build_dir, is_dry_run_cli=True)

        assert unique_tests == 0
        assert total_runs == 0
        assert est_seconds > 0  # Still estimates verified base stages (4s * impls)
