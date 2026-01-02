#include "../../include/sorts.h"

void bubble_sort(int *arr, int n) {
    int swapped;
    for (int i = 0; i < n - 1; i++) {
        swapped = 0;
        for (int j = 0; j < n - i - 1; j++) {
            if (arr[j] > arr[j + 1]) {
                int temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
                swapped = 1;
            }
        }
        if (!swapped) {
            break;
        }
    }
}

SortStats bubble_sort_stats(int *arr, int n) {
    SortStats stats = {0, 0};
    int swapped;

    for (int i = 0; i < n - 1; i++) {
        swapped = 0;
        for (int j = 0; j < n - i - 1; j++) {
            stats.comparisons++;
            if (arr[j] > arr[j + 1]) {
                int temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
                stats.swaps++;
                swapped = 1;
            }
        }
        // Early termination check (also a comparison in logic sense)
        if (!swapped) {
            break;
        }
    }

    return stats;
}
