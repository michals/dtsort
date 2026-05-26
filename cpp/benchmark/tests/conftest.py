import sys
from pathlib import Path
from unittest.mock import MagicMock

import pytest

# Add root benchmark directory to PATH so we can import modules
sys.path.insert(0, str(Path(__file__).parent.parent))

from runner.config import RunConfig
from runner.consts import BenchmarkImpl


@pytest.fixture
def base_config():
    """Returns a basic RunConfig for testing."""
    return RunConfig(
        max_n=10,
        count_iters=5,
        bench_iters=2,
        skip_build=True,
        impls=[BenchmarkImpl.GCC14],
        types="IntNaked",
        sizes="5:10:100",
        only_counts=True,
        inputs="random",
        adaptivity="adaptive",
        dry_run=True,
    )


@pytest.fixture
def mock_subprocess(mocker):
    """Mocks subprocess.run and subprocess.Popen globally."""
    mock_run = mocker.patch("subprocess.run")
    mock_popen = mocker.patch("subprocess.Popen")

    # Configure mock_run default return values
    mock_run.return_value = MagicMock(returncode=0, stdout="git_hash=abcdef12\n", stderr="")

    # Configure mock_popen default behavior
    mock_proc = MagicMock()
    mock_proc.returncode = 0
    mock_proc.stdout = ["Built target bench-GCC14-stock\n", "Built target bench-GCC14-dtsort\n"]
    mock_popen.return_value = mock_proc

    return mock_run, mock_popen
