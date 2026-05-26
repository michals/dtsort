"""
General utility functions and shared helpers for the benchmark orchestrator.

Provides a unified time-tracked subprocess runner (`execute_command`) and standard
formatted terminal output logging helpers.
"""

import contextlib
import subprocess
import sys
import time
from pathlib import Path


def execute_command(
    cmd: list[str],
    cwd: str | Path,
    log_file: str | Path | None = None,
    capture: bool = False,
) -> tuple[int, str]:
    """
    Executes a system shell command inside the specified working directory.

    Tracks elapsed execution time, handles real-time output logging to stdout and/or
    an optional log file, and raises an exception on non-zero exit status.

    Args:
        cmd: The command to run as a list of arguments (e.g. ["cmake", "--build", "build"]).
        cwd: The working directory in which the command should execute.
        log_file: An optional file path where stdout and stderr should be written.
        capture: If True, returns stdout and stderr of the command rather than printing it.

    Returns:
        A tuple of (exit_code, output_text).

    Raises:
        subprocess.CalledProcessError: If the process exits with a non-zero status.
    """
    start_time = time.perf_counter()
    cwd_path = Path(cwd)

    cmd_str = " ".join(cmd)
    print(f"[RUN] {cmd_str} (in {cwd_path.name})", flush=True)

    # Prepare log file path if requested
    if log_file:
        log_path = Path(log_file)
        log_path.parent.mkdir(parents=True, exist_ok=True)
        file_ctx = open(log_path, "a", encoding="utf-8")  # noqa: SIM115
    else:
        file_ctx = contextlib.nullcontext()

    with file_ctx as out_file:
        if out_file:
            out_file.write(f"\n--- [RUN] {cmd_str} at {time.strftime('%Y-%m-%d %H:%M:%S')} ---\n")
            out_file.flush()

        try:
            if capture:
                res = subprocess.run(
                    cmd,
                    cwd=cwd_path,
                    stdout=subprocess.PIPE,
                    stderr=subprocess.STDOUT,
                    text=True,
                    check=True,
                )
                elapsed = time.perf_counter() - start_time
                if out_file:
                    out_file.write(res.stdout)
                    out_file.write(f"\n--- [EXIT SUCCESS] elapsed: {elapsed:.2f}s ---\n")
                return res.returncode, res.stdout
            elif out_file:
                # Stream output directly to log file
                res = subprocess.run(
                    cmd,
                    cwd=cwd_path,
                    stdout=out_file,
                    stderr=subprocess.STDOUT,
                    check=True,
                )
                elapsed = time.perf_counter() - start_time
                out_file.write(f"\n--- [EXIT SUCCESS] elapsed: {elapsed:.2f}s ---\n")
                return res.returncode, ""
            else:
                # Normal execution, streaming directly to current stdout/stderr
                res = subprocess.run(
                    cmd,
                    cwd=cwd_path,
                    check=True,
                )
                return res.returncode, ""
        except subprocess.CalledProcessError as e:
            elapsed = time.perf_counter() - start_time
            err_msg = f"[ERROR] Command '{cmd_str}' failed with exit code {e.returncode} after {elapsed:.2f}s."
            print(err_msg, file=sys.stderr, flush=True)
            if out_file:
                out_file.write(f"\n--- [EXIT FAILURE] code: {e.returncode}, elapsed: {elapsed:.2f}s ---\n")
                out_file.flush()
            raise
