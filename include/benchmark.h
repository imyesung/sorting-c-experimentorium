#ifndef BENCHMARK_H
#define BENCHMARK_H

#include <stdbool.h>

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

double benchmark_sort(void (*sort_func)(int*, int), int *arr, int n);

void run_all_benchmarks(bool include_large_inputs);

void benchmark_by_size(void (*sort_func)(int*, int), const char *name,
                       DataPattern pattern, AlgorithmComplexity complexity,
                       bool include_large_inputs);

void benchmark_by_pattern(void (*sort_func)(int*, int), const char *name,
                          AlgorithmComplexity complexity,
                          bool include_large_inputs);

#endif
