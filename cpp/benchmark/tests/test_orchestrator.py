from pathlib import Path
from unittest.mock import MagicMock, patch

from main import get_git_hash
from runner.executor import estimate_runtime


def test_get_git_hash(mock_subprocess):
    mock_run, _ = mock_subprocess
    mock_run.return_value = MagicMock(returncode=0, stdout="abcdef12\n")

    git_hash = get_git_hash(Path("/fake/root"))
    assert git_hash == "abcdef12"


def test_estimate_runtime(base_config, capsys):
    with patch("subprocess.run") as mock_run:
        mock_run.return_value = MagicMock(returncode=0, stdout="test_case_1\ntest_case_2\n")

        estimate_runtime(base_config, Path("/fake/build"), is_dry_run_cli=True)

        captured = capsys.readouterr()
        assert "Expected Runtime" in captured.out
        assert "TOTAL EXPECTED RUN TIME" in captured.out
