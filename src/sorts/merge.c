#include <stdlib.h>
#include "../../include/sorts.h"

static void merge(int *arr, int left, int mid, int right) {
    int n1 = mid - left + 1;
    int n2 = right - mid;

    int *left_arr = (int *)malloc(n1 * sizeof(int));
    int *right_arr = (int *)malloc(n2 * sizeof(int));

    if (left_arr == NULL || right_arr == NULL) {
        free(left_arr);
        free(right_arr);
        return;
    }

    for (int i = 0; i < n1; i++) {
        left_arr[i] = arr[left + i];
    }
    for (int j = 0; j < n2; j++) {
        right_arr[j] = arr[mid + 1 + j];
    }

    int i = 0, j = 0, k = left;
    while (i < n1 && j < n2) {
        if (left_arr[i] <= right_arr[j]) {
            arr[k++] = left_arr[i++];
        } else {
            arr[k++] = right_arr[j++];
        }
    }

    while (i < n1) {
        arr[k++] = left_arr[i++];
    }
    while (j < n2) {
        arr[k++] = right_arr[j++];
    }

    free(left_arr);
    free(right_arr);
}

static void merge_sort_recursive(int *arr, int left, int right) {
    if (left >= right) {
        return;
    }
    int mid = left + (right - left) / 2;
    merge_sort_recursive(arr, left, mid);
    merge_sort_recursive(arr, mid + 1, right);
    merge(arr, left, mid, right);
}

void merge_sort(int *arr, int n) {
    if (arr == NULL || n <= 1) {
        return;
    }
    merge_sort_recursive(arr, 0, n - 1);
}
