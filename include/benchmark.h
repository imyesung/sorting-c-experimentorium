#ifndef BENCHMARK_H
#define BENCHMARK_H

#include <stdbool.h>

typedef enum {
    RANDOM,
    SORTED,
    REVERSE_SORTED,
    NEARLY_SORTED
} DataPattern;

double benchmark_sort(void (*sort_func)(int*, int), int *arr, int n);

void run_all_benchmarks(bool include_large_inputs);

void benchmark_by_size(void (*sort_func)(int*, int), const char *name,
                       DataPattern pattern, bool is_slow_algorithm,
                       bool include_large_inputs);

void benchmark_by_pattern(void (*sort_func)(int*, int), const char *name,
                          bool is_slow_algorithm,
                          bool include_large_inputs);

#endif
