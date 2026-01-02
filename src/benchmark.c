#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "../include/benchmark.h"
#include "../include/sorts.h"
#include "../include/data_generator.h"
#define ARRAY_SIZE(arr) (int)(sizeof(arr) / sizeof((arr)[0]))

/* ========== Sort Result Verification Functions ========== */

// Check if array is sorted in ascending order
static bool is_sorted(const int *arr, int n) {
    for (int i = 1; i < n; i++) {
        if (arr[i - 1] > arr[i]) {
            return false;
        }
    }
    return true;
}

// Comparison function for qsort
static int compare_int(const void *a, const void *b) {
    return (*(const int*)a - *(const int*)b);
}

// Verify that sort result matches the expected output (using qsort as reference)
static bool verify_sort_result(const int *result, const int *original, int n, const char *algo_name) {
    // Copy original and sort with qsort to create reference
    int *expected = (int*)malloc(n * sizeof(int));
    if (expected == NULL) {
        printf("[WARN] Memory allocation failed for verification\n");
        return true; // Skip verification on memory failure
    }
    memcpy(expected, original, n * sizeof(int));
    qsort(expected, n, sizeof(int), compare_int);

    // Compare results
    bool match = (memcmp(result, expected, n * sizeof(int)) == 0);
    
    if (!match) {
        printf("\n[ERROR] %s produced incorrect result!\n", algo_name);
        // Find first mismatch position
        for (int i = 0; i < n; i++) {
            if (result[i] != expected[i]) {
                printf("  First mismatch at index %d: got %d, expected %d\n", 
                       i, result[i], expected[i]);
                break;
            }
        }
    }

    free(expected);
    return match;
}

/* Practical default sizes for O(n log n) algorithms (<= 1,000,000) */
static const int DEFAULT_SIZES[] = {
    100,
    1000,
    5000,
    10000,
    50000,
    100000,
    500000,
    1000000
};

static const int EXTENDED_SIZES[] = {
    2000000,
    5000000,
    10000000,
    50000000,
    100000000
};

/* Separate size schedule for O(n^2) algorithms */
static const int QUADRATIC_SIZES[] = {
    100,
    1000,
    5000,
    10000,
    20000,
    100000
};

/* Pattern test sizes optimized for each complexity class */
#define PATTERN_SIZE_QUADRATIC     100000      // O(n^2)
#define PATTERN_SIZE_LINEARITHMIC  5000000    // O(n log n)
#define PATTERN_SIZE_LINEAR        50000000   // O(n)

static void counting_sort_wrapper(int *arr, int n) {
    if (arr == NULL || n <= 0) {
        return;
    }

    int max_val = arr[0];
    for (int i = 1; i < n; i++) {
        if (arr[i] > max_val) {
            max_val = arr[i];
        }
    }

    counting_sort(arr, n, max_val);
}

double benchmark_sort(void (*sort_func)(int*, int), int *arr, int n) {
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);
    sort_func(arr, n);
    clock_gettime(CLOCK_MONOTONIC, &end);

    uint64_t elapsed_ns = (uint64_t)(end.tv_sec - start.tv_sec) * 1000000000ULL
                        + (uint64_t)(end.tv_nsec - start.tv_nsec);
    return (double)elapsed_ns / 1e9;
}

BenchmarkResult benchmark_sort_stats(SortStats (*sort_func)(int*, int), int *arr, int n) {
    BenchmarkResult result = {0};
    struct timespec start, end;

    clock_gettime(CLOCK_MONOTONIC, &start);
    result.stats = sort_func(arr, n);
    clock_gettime(CLOCK_MONOTONIC, &end);

    result.time_ns = (uint64_t)(end.tv_sec - start.tv_sec) * 1000000000ULL
                   + (uint64_t)(end.tv_nsec - start.tv_nsec);
    result.time_sec = (double)result.time_ns / 1e9;

    return result;
}

static const char *pattern_names[] = {
    "Random",
    "Sorted",
    "ReverseSorted",
    "NearlySorted"
};

void benchmark_by_size(void (*sort_func)(int*, int), const char *name,
                       DataPattern pattern, AlgorithmComplexity complexity,
                       bool include_large_inputs) {
    FILE *fp = fopen("results/size_benchmark.csv", "a");
    if (fp == NULL) {
        printf("Error: Could not open results/size_benchmark.csv for writing\n");
        return;
    }

    const int *primary_sizes = (complexity == COMPLEXITY_QUADRATIC) ? QUADRATIC_SIZES : DEFAULT_SIZES;
    int primary_count = (complexity == COMPLEXITY_QUADRATIC) ? ARRAY_SIZE(QUADRATIC_SIZES)
                                                             : ARRAY_SIZE(DEFAULT_SIZES);

    const int *extra_sizes = (include_large_inputs && complexity != COMPLEXITY_QUADRATIC)
                                 ? EXTENDED_SIZES
                                 : NULL;
    int extra_count = extra_sizes ? ARRAY_SIZE(EXTENDED_SIZES) : 0;

    for (int series = 0; series < 2; series++) {
        const int *sizes = series == 0 ? primary_sizes : extra_sizes;
        int series_count = series == 0 ? primary_count : extra_count;

        if (sizes == NULL) {
            continue;
        }

        for (int i = 0; i < series_count; i++) {
            int n = sizes[i];
            int *arr = (int*)malloc(n * sizeof(int));
            int *original = (int*)malloc(n * sizeof(int));

            if (arr == NULL || original == NULL) {
                printf("Memory allocation failed for size %d\n", n);
                free(arr);
                free(original);
                continue;
            }

            generate_data(arr, n, pattern);
            memcpy(original, arr, n * sizeof(int));  // Keep original for verification

            printf("  Testing %s with %d elements (%s)...", name, n, pattern_names[pattern]);
            fflush(stdout);

            double time = benchmark_sort(sort_func, arr, n);

            // Verify sort correctness
            if (!is_sorted(arr, n)) {
                printf(" [FAIL - NOT SORTED]\n");
            } else if (!verify_sort_result(arr, original, n, name)) {
                printf(" [FAIL - WRONG RESULT]\n");
            } else {
                printf(" OK (%.4fs)\n", time);
            }

            fprintf(fp, "%s,%s,%d,%.6f\n", name, pattern_names[pattern], n, time);

            free(arr);
            free(original);
        }
    }

    fclose(fp);
}

// Get optimized pattern test size based on algorithm complexity
static int get_pattern_test_size(AlgorithmComplexity complexity, bool include_large_inputs) {
    if (!include_large_inputs) {
        // Conservative sizes when large inputs disabled
        switch (complexity) {
            case COMPLEXITY_QUADRATIC:    return 100000;
            case COMPLEXITY_LINEARITHMIC: return 100000;
            case COMPLEXITY_LINEAR:       return 1000000;
        }
    }
    // Full sizes for comprehensive benchmarking
    switch (complexity) {
        case COMPLEXITY_QUADRATIC:    return PATTERN_SIZE_QUADRATIC;
        case COMPLEXITY_LINEARITHMIC: return PATTERN_SIZE_LINEARITHMIC;
        case COMPLEXITY_LINEAR:       return PATTERN_SIZE_LINEAR;
    }
    return 100000; // fallback
}

void benchmark_by_pattern(void (*sort_func)(int*, int), const char *name,
                          AlgorithmComplexity complexity,
                          bool include_large_inputs) {
    FILE *fp = fopen("results/pattern_benchmark.csv", "a");
    if (fp == NULL) {
        printf("Error: Could not open results/pattern_benchmark.csv for writing\n");
        return;
    }

    DataPattern patterns[] = {RANDOM, SORTED, REVERSE_SORTED, NEARLY_SORTED};

    int size = get_pattern_test_size(complexity, include_large_inputs);

    printf("Testing %s with different patterns (size=%d):\n", name, size);

    for (int i = 0; i < 4; i++) {
        int *arr = (int*)malloc(size * sizeof(int));
        int *original = (int*)malloc(size * sizeof(int));

        if (arr == NULL || original == NULL) {
            printf("Memory allocation failed\n");
            free(arr);
            free(original);
            continue;
        }

        generate_data(arr, size, patterns[i]);
        memcpy(original, arr, size * sizeof(int));  // Keep original for verification

        printf("  Pattern: %s...", pattern_names[i]);
        fflush(stdout);

        double time = benchmark_sort(sort_func, arr, size);

        // Verify sort correctness
        if (!is_sorted(arr, size)) {
            printf(" [FAIL - NOT SORTED]\n");
        } else if (!verify_sort_result(arr, original, size, name)) {
            printf(" [FAIL - WRONG RESULT]\n");
        } else {
            printf(" OK (%.4fs)\n", time);
        }

        fprintf(fp, "%s,%s,%d,%.6f\n", name, pattern_names[i], size, time);

        free(arr);
        free(original);
    }

    fclose(fp);
}

void run_all_benchmarks(bool include_large_inputs) {
    // Initialize CSV files with headers
    FILE *fp1 = fopen("results/size_benchmark.csv", "w");
    if (fp1 == NULL) {
        printf("Error: Could not initialize results/size_benchmark.csv\n");
        return;
    }
    fprintf(fp1, "Algorithm,Pattern,Size,Time\n");
    fclose(fp1);

    FILE *fp2 = fopen("results/pattern_benchmark.csv", "w");
    if (fp2 == NULL) {
        printf("Error: Could not initialize results/pattern_benchmark.csv\n");
        return;
    }
    fprintf(fp2, "Algorithm,Pattern,Size,Time\n");
    fclose(fp2);

    printf("Starting comprehensive benchmark...\n\n");
    printf("Testing conditions:\n");
    printf("- Patterns: Random, Sorted, Reverse Sorted, Nearly Sorted\n");
    printf("- Baseline sizes: up to %d elements\n",
        DEFAULT_SIZES[ARRAY_SIZE(DEFAULT_SIZES) - 1]);
    if (include_large_inputs) {
     printf("- Extended sizes enabled: up to %d elements\n",
         EXTENDED_SIZES[ARRAY_SIZE(EXTENDED_SIZES) - 1]);
    } else {
     printf("- Extended sizes disabled (efficient algorithms limited to baseline)\n");
    }
    printf("\n");

    // O(n^2) algorithms - test with smaller sizes only
    printf("=== Testing O(n^2) Algorithms ===\n");
    printf("(Limited to %d elements max)\n\n",
        QUADRATIC_SIZES[ARRAY_SIZE(QUADRATIC_SIZES) - 1]);

    printf("[1/10] Selection Sort\n");
    benchmark_by_size(selection_sort, "SelectionSort", RANDOM, COMPLEXITY_QUADRATIC, include_large_inputs);
    benchmark_by_pattern(selection_sort, "SelectionSort", COMPLEXITY_QUADRATIC, include_large_inputs);

    printf("\n[2/10] Bubble Sort\n");
    benchmark_by_size(bubble_sort, "BubbleSort", RANDOM, COMPLEXITY_QUADRATIC, include_large_inputs);
    printf("  -> Best-case (sorted input) sweep\n");
    benchmark_by_size(bubble_sort, "BubbleSort", SORTED, COMPLEXITY_QUADRATIC, include_large_inputs);
    benchmark_by_pattern(bubble_sort, "BubbleSort", COMPLEXITY_QUADRATIC, include_large_inputs);

    printf("\n[3/10] Insertion Sort\n");
    benchmark_by_size(insertion_sort, "InsertionSort", RANDOM, COMPLEXITY_QUADRATIC, include_large_inputs);
    printf("  -> Best-case (sorted input) sweep\n");
    benchmark_by_size(insertion_sort, "InsertionSort", SORTED, COMPLEXITY_QUADRATIC, include_large_inputs);
    benchmark_by_pattern(insertion_sort, "InsertionSort", COMPLEXITY_QUADRATIC, include_large_inputs);

    // O(n log n) algorithms - test with all sizes
    printf("\n=== Testing O(n log n) Algorithms ===\n");
    printf("(Default limit %d; extended limit %d when enabled)\n\n",
        DEFAULT_SIZES[ARRAY_SIZE(DEFAULT_SIZES) - 1],
        EXTENDED_SIZES[ARRAY_SIZE(EXTENDED_SIZES) - 1]);

    printf("[4/10] Merge Sort\n");
    benchmark_by_size(merge_sort, "MergeSort", RANDOM, COMPLEXITY_LINEARITHMIC, include_large_inputs);
    benchmark_by_pattern(merge_sort, "MergeSort", COMPLEXITY_LINEARITHMIC, include_large_inputs);

    printf("\n[5/10] Quick Sort\n");
    benchmark_by_size(quick_sort, "QuickSort", RANDOM, COMPLEXITY_LINEARITHMIC, include_large_inputs);
    benchmark_by_pattern(quick_sort, "QuickSort", COMPLEXITY_LINEARITHMIC, include_large_inputs);

    printf("\n[6/10] Heap Sort\n");
    benchmark_by_size(heap_sort, "HeapSort", RANDOM, COMPLEXITY_LINEARITHMIC, include_large_inputs);
    benchmark_by_pattern(heap_sort, "HeapSort", COMPLEXITY_LINEARITHMIC, include_large_inputs);

    // Special algorithms
    printf("\n=== Testing Special Algorithms ===\n\n");

    printf("[7/10] Shell Sort\n");
    benchmark_by_size(shell_sort, "ShellSort", RANDOM, COMPLEXITY_LINEARITHMIC, include_large_inputs);
    benchmark_by_pattern(shell_sort, "ShellSort", COMPLEXITY_LINEARITHMIC, include_large_inputs);

    printf("\n[8/10] Counting Sort\n");
    benchmark_by_size(counting_sort_wrapper, "CountingSort", RANDOM, COMPLEXITY_LINEAR, include_large_inputs);
    benchmark_by_pattern(counting_sort_wrapper, "CountingSort", COMPLEXITY_LINEAR, include_large_inputs);

    printf("\n[9/10] Radix Sort\n");
    benchmark_by_size(radix_sort, "RadixSort", RANDOM, COMPLEXITY_LINEAR, include_large_inputs);
    benchmark_by_pattern(radix_sort, "RadixSort", COMPLEXITY_LINEAR, include_large_inputs);

    printf("\n[10/10] Bucket Sort\n");
    benchmark_by_size(bucket_sort, "BucketSort", RANDOM, COMPLEXITY_LINEAR, include_large_inputs);
    benchmark_by_pattern(bucket_sort, "BucketSort", COMPLEXITY_LINEAR, include_large_inputs);

    printf("\n=== All Benchmarks Completed ===\n");
}

void run_stats_benchmark(void) {
    printf("=== Bubble Sort Statistics Benchmark ===\n\n");

    FILE *fp = fopen("results/stats_benchmark.csv", "w");
    if (fp == NULL) {
        printf("Error: Could not open results/stats_benchmark.csv for writing\n");
        return;
    }
    fprintf(fp, "Algorithm,Pattern,Size,Time,Comparisons,Swaps\n");

    const char *pattern_names[] = {"Random", "Sorted", "ReverseSorted", "NearlySorted"};
    DataPattern patterns[] = {RANDOM, SORTED, REVERSE_SORTED, NEARLY_SORTED};
    int sizes[] = {100, 1000, 5000, 10000, 20000, 50000};
    int num_sizes = sizeof(sizes) / sizeof(sizes[0]);

    for (int p = 0; p < 4; p++) {
        printf("Pattern: %s\n", pattern_names[p]);
        for (int s = 0; s < num_sizes; s++) {
            int size = sizes[s];
            int *arr = (int*)malloc(size * sizeof(int));
            if (arr == NULL) {
                printf("  Memory allocation failed for size %d\n", size);
                continue;
            }

            generate_data(arr, size, patterns[p]);
            BenchmarkResult result = benchmark_sort_stats(bubble_sort_stats, arr, size);

            printf("  n=%6d: time=%.6fs, comparisons=%llu, swaps=%llu\n",
                   size, result.time_sec,
                   (unsigned long long)result.stats.comparisons,
                   (unsigned long long)result.stats.swaps);

            fprintf(fp, "BubbleSort,%s,%d,%.9f,%llu,%llu\n",
                    pattern_names[p], size, result.time_sec,
                    (unsigned long long)result.stats.comparisons,
                    (unsigned long long)result.stats.swaps);

            free(arr);
        }
        printf("\n");
    }

    fclose(fp);
    printf("Results saved to results/stats_benchmark.csv\n");
}
