# Sorting Algorithm Performance Benchmark

## Overview
Ready-to-run benchmark suite for 10 sorting algorithms, covering multiple input sizes (100 up to 100 M) and four data patterns (random, sorted, reverse, nearly sorted). Results are exported as CSV files and gnuplot-generated PNG charts for direct comparison.

## Project Structure
```
sorting-c-experimentorium/
├── include/
│   ├── benchmark.h      # Benchmark driver API
│   ├── data_generator.h # Test data patterns
│   ├── sorts.h          # Sorting algorithm declarations
│   └── visualizer.h     # Plot generation helpers
├── src/
│   ├── benchmark.c      # Size/pattern benchmark orchestration
│   ├── data_generator.c # Test data builders
│   ├── visualizer.c     # gnuplot wrappers
│   └── sorts/           # Algorithm implementations
│       ├── bubble.c     ├── bucket.c    ├── counting.c  ├── heap.c
│       ├── insertion.c  ├── merge.c     ├── quick.c     ├── radix.c
│       ├── selection.c  └── shell.c
├── results/             # CSV + PNG outputs (generated)
├── main.c               # CLI entry point
├── Makefile
└── README.md
```

## Benchmark Coverage

### Input Sizes
- Quadratic algorithms (Selection, Bubble, Insertion – random data): 100, 1 000, 5 000, 10 000, 20 000, 30 000, 40 000, 50 000  
- Quadratic best case (sorted input for Bubble/Insertion): same as above  
- Efficient & special algorithms (Merge, Quick, Heap, Shell, Counting, Radix, Bucket): 100, 1 000, 5 000, 10 000, 50 000, 100 000, 500 000, 1 000 000, 2 000 000, 5 000 000, 10 000 000, 50 000 000, 100 000 000

### Data Patterns
- **Random** – average-case workload
- **Sorted** – best case for insertion/bubble style algorithms
- **ReverseSorted** – worst case for naive quick/insertion/bubble
- **NearlySorted** – 5 % elements randomly swapped

### Algorithms Implemented
Selection, Bubble, Insertion, Merge, Quick (random pivot), Heap, Shell (Ciura gaps), Counting, Radix (base 10), Bucket (10 buckets).

## Build & Run

### Prerequisites
```bash
brew install gnuplot        # macOS
make install-deps           # optional helper alias
```

### Commands
```bash
make                 # build binary into ./bin/benchmark
./bin/benchmark      # full benchmark (includes large sizes)
./bin/benchmark --no-large-sizes   # restrict to ≤1 M elements
./bin/benchmark --plot-only        # regenerate PNG from existing CSV
make clean           # remove obj/bin
make cleanall        # remove obj/bin/results
```

## Outputs

### CSV
- `results/size_benchmark.csv` – (Algorithm, Pattern, Size, Time)
- `results/pattern_benchmark.csv` – pattern sweep per algorithm (includes tested size)

### PNG Charts
- `1_basic_sorts_log.png`, `1_basic_sorts_linear.png`
- `1_basic_sorts_best_linear.png` (sorted-input best case)
- `2_efficient_sorts_log.png`, `2_efficient_sorts_linear.png`
- `3_special_sorts_log.png`, `3_special_sorts_linear.png`
- `pattern_comparison_quadratic.png` (O(n²) algorithms)
- `pattern_comparison_efficient.png` (O(n log n) algorithms)
- `pattern_comparison_special.png` (non-comparison algorithms)

Each size plot provides log/linear axis variants plus reference curves (O(n²), O(n log n), or O(n) depending on the group).

## Notes
- Large-input runs (≥50 M) allocate hundreds of MB; ensure sufficient RAM.
- Timing uses `clock()` (single-threaded CPU time).
- CLI flag `--include-large-sizes` is implicit; use `--no-large-sizes` for quick iterative testing.