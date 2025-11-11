#ifndef SORTS_H
#define SORTS_H

// Basic sorting algorithms - O(n^2)
void selection_sort(int *arr, int n);
void bubble_sort(int *arr, int n);
void insertion_sort(int *arr, int n);

// Advanced sorting algorithms - O(n log n)
void merge_sort(int *arr, int n);
void quick_sort(int *arr, int n);
void heap_sort(int *arr, int n);

// Special sorting algorithms
void shell_sort(int *arr, int n);           // O(n^(3/2)) or better
void counting_sort(int *arr, int n, int max_val);  // O(n + k)
void radix_sort(int *arr, int n);           // O(d * (n + k))
void bucket_sort(int *arr, int n);          // O(n + k)

#endif
