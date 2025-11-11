#include <stdlib.h>
#include "../../include/sorts.h"

static void swap(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

static int partition(int *arr, int low, int high) {
    int pivot_index = low + rand() % (high - low + 1);
    int pivot_value = arr[pivot_index];
    swap(&arr[pivot_index], &arr[high]);

    int i = low - 1;
    for (int j = low; j < high; j++) {
        if (arr[j] <= pivot_value) {
            i++;
            swap(&arr[i], &arr[j]);
        }
    }
    swap(&arr[i + 1], &arr[high]);
    return i + 1;
}

static void quick_sort_recursive(int *arr, int low, int high) {
    while (low < high) {
        int pivot = partition(arr, low, high);
        if (pivot - low < high - pivot) {
            quick_sort_recursive(arr, low, pivot - 1);
            low = pivot + 1;
        } else {
            quick_sort_recursive(arr, pivot + 1, high);
            high = pivot - 1;
        }
    }
}

void quick_sort(int *arr, int n) {
    if (arr == NULL || n <= 1) {
        return;
    }
    quick_sort_recursive(arr, 0, n - 1);
}
