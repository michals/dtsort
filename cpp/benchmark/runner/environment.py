"""
Environment auditing and dynamic compilation bypass heuristics.

Validates the Git working tree state and compiled binary timestamps to dynamically
skip long compilation sequences if the source tree has not changed.
"""

import subprocess
from pathlib import Path

from runner.config import RunConfig, get_variants


def check_auto_skip_build(config: RunConfig, build_dir: Path, root_dir: Path) -> bool:
    """
    Heuristically determines if standard C++ target compilation can be safely bypassed.

    Bypasses the build ONLY if:
    1. The CMake cache (`CMakeCache.txt`) exists.
    2. The Git worktree is completely clean of modified tracked files (ignoring untracked ones).
    3. All required target executables exist and are newer than the latest Git commit.
    4. The compiled maximum `N` size matches the currently requested configuration.

    Args:
        config: The RunConfig configuration instance.
        build_dir: Path to the CMake C++ build directory.
        root_dir: Path to the project's root Git directory.

    Returns:
        True if all conditions are met and compilation can be bypassed; False otherwise.
    """
    cmake_cache = build_dir / "CMakeCache.txt"
    if not cmake_cache.exists():
        return False

    # 1. Check if Git status has any local changes (ignoring untracked files)
    try:
        status_out = subprocess.check_output(["git", "status", "--porcelain", "-uno"], cwd=root_dir, text=True).strip()
        if status_out:
            return False  # Local modifications exist, must rebuild
    except Exception:
        return False

    # 2. Check HEAD commit timestamp
    try:
        commit_ts = int(subprocess.check_output(["git", "log", "-1", "--format=%ct"], cwd=root_dir, text=True).strip())
    except Exception:
        return False

    # 3. Check if all required executables exist and are up-to-date
    run_tasks = []
    for impl in config.impls:
        for variant in get_variants(impl, config.adaptivity):
            run_tasks.append(f"bench-{impl}-{variant}")

    for bin_name in run_tasks:
        exe = build_dir / bin_name
        if not exe.exists() or exe.stat().st_mtime < commit_ts:
            return False  # Missing or stale binary, must rebuild

    # 4. Check if max_n matches the compile parameters
    compiled_max_n = None
    try:
        for line in cmake_cache.read_text(encoding="utf-8").splitlines():
            if line.startswith("DTSORT_MAX_N:STRING="):
                try:
                    compiled_max_n = int(line.strip().split("=")[1])
                except (ValueError, IndexError):
                    pass
                break
    except Exception:
        return False

    if compiled_max_n != config.max_n:
        return False  # Parameter mismatch, must rebuild

    return True
