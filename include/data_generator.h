#ifndef DATA_GENERATOR_H
#define DATA_GENERATOR_H

#include "benchmark.h"

/**
 * Generate random data
 * Values range from 0 to 999999
 */
void generate_random(int *arr, int n);

/**
 * Generate sorted data (ascending order)
 * Best case for: Insertion Sort, Bubble Sort (optimized)
 */
void generate_sorted(int *arr, int n);

/**
 * Generate reverse sorted data (descending order)
 * Worst case for: Quick Sort (bad pivot), Insertion Sort, Bubble Sort
 */
void generate_reverse_sorted(int *arr, int n);

/**
 * Generate nearly sorted data
 * Sorted array with some random swaps (5% of array size)
 * Tests algorithms' adaptability to partially sorted data
 */
void generate_nearly_sorted(int *arr, int n, int swaps);

/**
 * Unified data generation function
 * @param arr: target array
 * @param n: array size
 * @param pattern: desired data pattern
 */
void generate_data(int *arr, int n, DataPattern pattern);

/**
 * Copy array for benchmark purposes
 * Ensures each algorithm tests the same data
 */
void copy_array(int *dest, int *src, int n);

#endif
