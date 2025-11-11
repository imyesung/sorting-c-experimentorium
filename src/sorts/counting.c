#include <stdlib.h>
#include <string.h>
#include "../../include/sorts.h"

void counting_sort(int *arr, int n, int max_val) {
    if (arr == NULL || n <= 1 || max_val < 0) {
        return;
    }

    int range = max_val + 1;
    int *count = (int *)calloc(range, sizeof(int));
    int *output = (int *)malloc(n * sizeof(int));

    if (count == NULL || output == NULL) {
        free(count);
        free(output);
        return;
    }

    for (int i = 0; i < n; i++) {
        if (arr[i] < 0) {
            free(count);
            free(output);
            return;
        }
        count[arr[i]]++;
    }

    for (int i = 1; i < range; i++) {
        count[i] += count[i - 1];
    }

    for (int i = n - 1; i >= 0; i--) {
        output[count[arr[i]] - 1] = arr[i];
        count[arr[i]]--;
    }

    for (int i = 0; i < n; i++) {
        arr[i] = output[i];
    }

    free(count);
    free(output);
}
