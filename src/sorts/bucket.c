#include <stdlib.h>
#include <string.h>
#include "../../include/sorts.h"

typedef struct {
    int *data;
    int count;
    int capacity;
} Bucket;

static void bucket_init(Bucket *bucket, int initial_capacity) {
    bucket->data = (int *)malloc(initial_capacity * sizeof(int));
    bucket->count = 0;
    bucket->capacity = bucket->data ? initial_capacity : 0;
}

static void bucket_push(Bucket *bucket, int value) {
    if (bucket->count >= bucket->capacity) {
        int new_capacity = bucket->capacity == 0 ? 8 : bucket->capacity * 2;
        int *new_data = (int *)realloc(bucket->data, new_capacity * sizeof(int));
        if (new_data == NULL) {
            return;
        }
        bucket->data = new_data;
        bucket->capacity = new_capacity;
    }
    bucket->data[bucket->count++] = value;
}

static int compare_ints(const void *a, const void *b) {
    int lhs = *(const int *)a;
    int rhs = *(const int *)b;
    return (lhs > rhs) - (lhs < rhs);
}

static void bucket_free(Bucket *bucket) {
    free(bucket->data);
    bucket->data = NULL;
    bucket->count = 0;
    bucket->capacity = 0;
}

void bucket_sort(int *arr, int n) {
    if (arr == NULL || n <= 1) {
        return;
    }

    int min_val = arr[0];
    int max_val = arr[0];
    for (int i = 1; i < n; i++) {
        if (arr[i] < min_val) {
            min_val = arr[i];
        }
        if (arr[i] > max_val) {
            max_val = arr[i];
        }
    }

    if (min_val == max_val) {
        return;
    }

    int bucket_count = n < 1024 ? n : 1024;
    Bucket *buckets = (Bucket *)malloc(bucket_count * sizeof(Bucket));
    if (buckets == NULL) {
        return;
    }

    for (int i = 0; i < bucket_count; i++) {
        bucket_init(&buckets[i], 8);
    }

    double interval = (double)(max_val - min_val + 1) / bucket_count;
    if (interval == 0.0) {
        interval = 1.0;
    }

    for (int i = 0; i < n; i++) {
        int index = (int)((arr[i] - min_val) / interval);
        if (index >= bucket_count) {
            index = bucket_count - 1;
        }
        bucket_push(&buckets[index], arr[i]);
    }

    int pos = 0;
    for (int i = 0; i < bucket_count; i++) {
        if (buckets[i].count > 0) {
            qsort(buckets[i].data, buckets[i].count, sizeof(int), compare_ints);
            memcpy(&arr[pos], buckets[i].data, buckets[i].count * sizeof(int));
            pos += buckets[i].count;
        }
        bucket_free(&buckets[i]);
    }

    free(buckets);
}
