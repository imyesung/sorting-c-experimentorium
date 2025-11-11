#include <stdlib.h>
#include <time.h>
#include "../include/data_generator.h"

void generate_random(int *arr, int n) {
    // Generate random integers from 0 to 999,999
    for (int i = 0; i < n; i++) {
        arr[i] = rand() % 1000000;
    }
}

void generate_sorted(int *arr, int n) {
    // Generate ascending sequence
    // Best case for: Insertion Sort, Bubble Sort (with optimization)
    for (int i = 0; i < n; i++) {
        arr[i] = i;
    }
}

void generate_reverse_sorted(int *arr, int n) {
    // Generate descending sequence
    // Worst case for: Quick Sort (with bad pivot selection)
    // Worst case for: Insertion Sort, Bubble Sort
    for (int i = 0; i < n; i++) {
        arr[i] = n - i;
    }
}

void generate_nearly_sorted(int *arr, int n, int swaps) {
    // Start with sorted array
    generate_sorted(arr, n);

    // Perform random swaps (default: 5% of array size)
    // Tests adaptability to partially sorted data
    for (int i = 0; i < swaps; i++) {
        int idx1 = rand() % n;
        int idx2 = rand() % n;

        // Swap elements
        int temp = arr[idx1];
        arr[idx1] = arr[idx2];
        arr[idx2] = temp;
    }
}

void generate_data(int *arr, int n, DataPattern pattern) {
    // Seed random number generator
    static int seeded = 0;
    if (!seeded) {
        srand((unsigned int)time(NULL));
        seeded = 1;
    }

    switch(pattern) {
        case RANDOM:
            generate_random(arr, n);
            break;
        case SORTED:
            generate_sorted(arr, n);
            break;
        case REVERSE_SORTED:
            generate_reverse_sorted(arr, n);
            break;
        case NEARLY_SORTED:
            // Use 5% swaps for nearly sorted data
            generate_nearly_sorted(arr, n, n / 20);
            break;
    }
}
