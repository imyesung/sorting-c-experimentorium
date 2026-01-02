#ifndef BENCHMARK_H
#define BENCHMARK_H

#include <stdbool.h>
#include <stdint.h>
#include "sorts.h"

typedef enum {
    RANDOM,
    SORTED,
    REVERSE_SORTED,
    NEARLY_SORTED
} DataPattern;

// Algorithm complexity categories for optimized benchmarking
typedef enum {
    COMPLEXITY_QUADRATIC,   // O(n^2): Selection, Bubble, Insertion
    COMPLEXITY_LINEARITHMIC, // O(n log n): Merge, Quick, Heap, Shell
    COMPLEXITY_LINEAR       // O(n): Counting, Radix, Bucket
} AlgorithmComplexity;

// Benchmark result with nanosecond precision and operation counts
typedef struct {
    double time_sec;         // Time in seconds
    uint64_t time_ns;        // Time in nanoseconds
    SortStats stats;         // Comparison and swap counts
} BenchmarkResult;

double benchmark_sort(void (*sort_func)(int*, int), int *arr, int n);

// Enhanced benchmark with statistics (nanosecond precision)
BenchmarkResult benchmark_sort_stats(SortStats (*sort_func)(int*, int), int *arr, int n);

void run_all_benchmarks(bool include_large_inputs);

void benchmark_by_size(void (*sort_func)(int*, int), const char *name,
                       DataPattern pattern, AlgorithmComplexity complexity,
                       bool include_large_inputs);

void benchmark_by_pattern(void (*sort_func)(int*, int), const char *name,
                          AlgorithmComplexity complexity,
                          bool include_large_inputs);

// Run stats benchmark (comparison/swap counts)
void run_stats_benchmark(void);

#endif
