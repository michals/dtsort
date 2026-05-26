import argparse

import pytest

from runner.config import generate_log_sizes, parse_sizes


def test_generate_log_sizes_basic():
    sizes = generate_log_sizes(5, 10, 100)
    assert len(sizes) == 5
    assert sizes[0] == 10
    assert sizes[-1] == 100
    assert sorted(sizes) == sizes


def test_generate_log_sizes_single_element():
    sizes = generate_log_sizes(1, 10, 100)
    assert sizes == [10]


def test_generate_log_sizes_uniqueness():
    sizes = generate_log_sizes(10, 2, 15)
    assert len(sizes) == 10
    assert len(set(sizes)) == 10


def test_parse_sizes_explicit():
    assert parse_sizes("10,20,30") == [10, 20, 30]
    assert parse_sizes("   10, 20 , 30   ") == [10, 20, 30]


def test_parse_sizes_geometric():
    parsed = parse_sizes("5:10:100")
    assert len(parsed) == 5
    assert parsed[0] == 10
    assert parsed[-1] == 100


def test_parse_sizes_invalid_format():
    with pytest.raises(argparse.ArgumentTypeError):
        parse_sizes("5:10")

    with pytest.raises(argparse.ArgumentTypeError):
        parse_sizes("5:10:100:200")

    with pytest.raises(argparse.ArgumentTypeError):
        parse_sizes("abc:10:100")
