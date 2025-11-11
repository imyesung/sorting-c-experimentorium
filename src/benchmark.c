#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "../include/benchmark.h"
#include "../include/sorts.h"
#include "../include/data_generator.h"
#define ARRAY_SIZE(arr) (int)(sizeof(arr) / sizeof((arr)[0]))

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
    50000
};

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
    clock_t start = clock();
    sort_func(arr, n);
    clock_t end = clock();
    return ((double)(end - start)) / CLOCKS_PER_SEC;
}

static const char *pattern_names[] = {
    "Random",
    "Sorted",
    "ReverseSorted",
    "NearlySorted"
};

void benchmark_by_size(void (*sort_func)(int*, int), const char *name,
                       DataPattern pattern, bool is_slow_algorithm,
                       bool include_large_inputs) {
    FILE *fp = fopen("results/size_benchmark.csv", "a");
    if (fp == NULL) {
        printf("Error: Could not open results/size_benchmark.csv for writing\n");
        return;
    }

    const int *primary_sizes = is_slow_algorithm ? QUADRATIC_SIZES : DEFAULT_SIZES;
    int primary_count = is_slow_algorithm ? ARRAY_SIZE(QUADRATIC_SIZES)
                                          : ARRAY_SIZE(DEFAULT_SIZES);

    const int *extra_sizes = include_large_inputs && !is_slow_algorithm
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

        if (arr == NULL) {
            printf("Memory allocation failed for size %d\n", n);
            continue;
        }

        generate_data(arr, n, pattern);

            printf("  Testing %s with %d elements (%s)...\n",
                   name, n, pattern_names[pattern]);

            double time = benchmark_sort(sort_func, arr, n);

            fprintf(fp, "%s,%s,%d,%.6f\n", name, pattern_names[pattern], n, time);

            free(arr);
        }
    }

    fclose(fp);
}

void benchmark_by_pattern(void (*sort_func)(int*, int), const char *name, int size) {
    FILE *fp = fopen("results/pattern_benchmark.csv", "a");
    if (fp == NULL) {
        printf("Error: Could not open results/pattern_benchmark.csv for writing\n");
        return;
    }

    DataPattern patterns[] = {RANDOM, SORTED, REVERSE_SORTED, NEARLY_SORTED};

    printf("Testing %s with different patterns (size=%d):\n", name, size);

    for (int i = 0; i < 4; i++) {
        int *arr = (int*)malloc(size * sizeof(int));

        if (arr == NULL) {
            printf("Memory allocation failed\n");
            continue;
        }

        generate_data(arr, size, patterns[i]);

        printf("  Pattern: %s...\n", pattern_names[i]);

        double time = benchmark_sort(sort_func, arr, size);
        fprintf(fp, "%s,%s,%d,%.6f\n", name, pattern_names[i], size, time);

        free(arr);
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
    benchmark_by_size(selection_sort, "SelectionSort", RANDOM, true, include_large_inputs);
    benchmark_by_pattern(selection_sort, "SelectionSort", 10000);

    printf("\n[2/10] Bubble Sort\n");
    benchmark_by_size(bubble_sort, "BubbleSort", RANDOM, true, include_large_inputs);
    printf("  -> Best-case (sorted input) sweep\n");
    benchmark_by_size(bubble_sort, "BubbleSort", SORTED, true, include_large_inputs);
    benchmark_by_pattern(bubble_sort, "BubbleSort", 10000);

    printf("\n[3/10] Insertion Sort\n");
    benchmark_by_size(insertion_sort, "InsertionSort", RANDOM, true, include_large_inputs);
    printf("  -> Best-case (sorted input) sweep\n");
    benchmark_by_size(insertion_sort, "InsertionSort", SORTED, true, include_large_inputs);
    benchmark_by_pattern(insertion_sort, "InsertionSort", 10000);

    // O(n log n) algorithms - test with all sizes
    printf("\n=== Testing O(n log n) Algorithms ===\n");
    printf("(Default limit %d; extended limit %d when enabled)\n\n",
        DEFAULT_SIZES[ARRAY_SIZE(DEFAULT_SIZES) - 1],
        EXTENDED_SIZES[ARRAY_SIZE(EXTENDED_SIZES) - 1]);

    printf("[4/10] Merge Sort\n");
    benchmark_by_size(merge_sort, "MergeSort", RANDOM, false, include_large_inputs);
    benchmark_by_pattern(merge_sort, "MergeSort", 1000000);

    printf("\n[5/10] Quick Sort\n");
    benchmark_by_size(quick_sort, "QuickSort", RANDOM, false, include_large_inputs);
    benchmark_by_pattern(quick_sort, "QuickSort", 1000000);

    printf("\n[6/10] Heap Sort\n");
    benchmark_by_size(heap_sort, "HeapSort", RANDOM, false, include_large_inputs);
    benchmark_by_pattern(heap_sort, "HeapSort", 1000000);

    // Special algorithms
    printf("\n=== Testing Special Algorithms ===\n\n");

    printf("[7/10] Shell Sort\n");
    benchmark_by_size(shell_sort, "ShellSort", RANDOM, false, include_large_inputs);
    benchmark_by_pattern(shell_sort, "ShellSort", 1000000);

    printf("\n[8/10] Counting Sort\n");
    benchmark_by_size(counting_sort_wrapper, "CountingSort", RANDOM, false, include_large_inputs);
    benchmark_by_pattern(counting_sort_wrapper, "CountingSort", 1000000);

    printf("\n[9/10] Radix Sort\n");
    benchmark_by_size(radix_sort, "RadixSort", RANDOM, false, include_large_inputs);
    benchmark_by_pattern(radix_sort, "RadixSort", 1000000);

    printf("\n[10/10] Bucket Sort\n");
    benchmark_by_size(bucket_sort, "BucketSort", RANDOM, false, include_large_inputs);
    benchmark_by_pattern(bucket_sort, "BucketSort", 1000000);

    printf("\n=== All Benchmarks Completed ===\n");
}
