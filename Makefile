.PHONY: all examples bench bench-fast help

# Default target: show help
all: help

# Build and run the C++ integration examples
examples:
	cmake -B cpp/examples/build -S cpp/examples && cmake --build cpp/examples/build
	@echo "--- simple_sort ---"
	./cpp/examples/build/simple_sort
	@echo "--- simple_benchmark ---"
	./cpp/examples/build/simple_benchmark

# Run the default benchmark: standalone dt_stable_sort, 50 log-spaced sizes from 20 to 100K
# Requires: Python 3.11+, uv, CMake 3.16+, C++20 compiler, internet (Google Benchmark fetch)
# On Windows: DT-only mode is used automatically (no Unix shell scripts needed)
# Full sweep note: a complete run can take several hours on Apple M4. This default is scoped
# to ~30 minutes. Use 'uv run main.py --help' inside cpp/benchmark/ to customise scope.
bench:
	uv --directory cpp/benchmark run main.py --impls DT --sizes 50:20:100000

# Quick operations-count sweep (no wall-clock timing, N=2..6, fast)
bench-fast:
	uv --directory cpp/benchmark run main.py --impls DT --only-counts --count-iters 2 -n 6

# Print a helpful usage description
help:
	@echo "Available Makefile targets:"
	@echo "  make examples   - Build and run the C++ usage examples"
	@echo "  make bench      - Run the default benchmark (dt_stable_sort, ~30 min)"
	@echo "  make bench-fast - Run a quick operation-count sweep (no wall-clock timing)"
	@echo "  make help       - Show this help message"
	@echo ""
	@echo "For advanced benchmark options, run:"
	@echo "  uv run main.py --help    (from cpp/benchmark/ directory)"
