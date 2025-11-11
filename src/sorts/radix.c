#include <stdlib.h>
#include "../../include/sorts.h"

static int get_max(int *arr, int n) {
    int max_val = arr[0];
    for (int i = 1; i < n; i++) {
        if (arr[i] > max_val) {
            max_val = arr[i];
        }
    }
    return max_val;
}

static void counting_sort_by_digit(int *arr, int n, int exp) {
    int output_size = n;
    int *output = (int *)malloc(output_size * sizeof(int));
    int count[10] = {0};

    if (output == NULL) {
        return;
    }

    for (int i = 0; i < n; i++) {
        int digit = (arr[i] / exp) % 10;
        count[digit]++;
    }

    for (int i = 1; i < 10; i++) {
        count[i] += count[i - 1];
    }

    for (int i = n - 1; i >= 0; i--) {
        int digit = (arr[i] / exp) % 10;
        output[count[digit] - 1] = arr[i];
        count[digit]--;
    }

    for (int i = 0; i < n; i++) {
        arr[i] = output[i];
    }

    free(output);
}

void radix_sort(int *arr, int n) {
    if (arr == NULL || n <= 1) {
        return;
    }

    for (int i = 0; i < n; i++) {
        if (arr[i] < 0) {
            return;
        }
    }

    int max_val = get_max(arr, n);

    for (int exp = 1; max_val / exp > 0; exp *= 10) {
        counting_sort_by_digit(arr, n, exp);
    }
}
