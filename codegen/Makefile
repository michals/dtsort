.PHONY: all build generate test clean help

# Default target: configure, build, and run stable sort correctness tests
all: build test

# Configure and compile the core codegen tools and test targets
build:
	cmake -B build && cmake --build build

# Generate all optimal dtsort headers and pure theoretical metrics (N=2..10)
generate: build
	cmake --build build --target generate

# Run dt_stable_sort correctness and stability verification tests
test: build
	./build/test-dt-stable-sort

# Clean build directory and all generated artifacts
clean:
	@if [ -d build ]; then cmake --build build --target clean-generated || true; fi
	rm -rf build

# Print a helpful usage description
help:
	@echo "Available Makefile targets:"
	@echo "  make build        - Configure and compile the core codegen tools"
	@echo "  make generate     - Generate all optimal dtsort sorting headers (N=2..10)"
	@echo "  make test         - Run dt_stable_sort correctness and stability tests"
	@echo "  make clean        - Remove all build directories and generated artifacts"
	@echo "  make help         - Show this help message"
