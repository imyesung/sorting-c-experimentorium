# Sorting Algorithm Performance Benchmark Report

## 1. Goals

- Implement a C version of 10 sorting algorithms
- Benchmark them under controlled conditions (same machine, same generator, same timing API).
- Visualize how time complexity behaves in practice:
  - $O(n^2)$ vs $O(n \log n)$ vs $O(n)$ non-comparison sorts.
  - Sensitivity to input order: random / already sorted / reverse sorted / nearly sorted.
- Verify correctness: all algorithms must produce identical sorted output for the same input.

## 2. Algorithms & Input Models

### 2.1 Algorithms Implemented

All algorithms share the same `void sort(int *arr, int n)`-style API behind `sorts.h`, and are selected by name in the benchmark.

* Quadratic comparison sorts ($O(n^2)$ in the general case)
  * Selection sort (`selection.c`)
  * Bubble sort (`bubble.c`)
  * Insertion sort (`insertion.c`)

* Efficient comparison sorts ($O(n \log n)$ on average / worst)
  * Merge sort (`merge.c`)
  * Quick sort with random pivot (`quick.c`)
  * Heap sort (`heap.c`)
  * Shell sort with standard gap sequence ($n/2$) (`shell.c`)

* Special / non-comparison-based sorts (roughly linear in range/keys)
  * Counting sort (`counting.c`)
  * Radix sort (LSD, base 10) (`radix.c`)
  * Bucket sort with 10 buckets (`bucket.c`)

### 2.2 Input Sizes

Different size grids are used so that each complexity class is pushed to the regime where its behavior is visible.

* For quadratic algorithms (Selection, Bubble, Insertion)
  * `100, 1000, 5000, 10000, 20000, 50000`

* For efficient comparison algorithms (Merge, Quick, Heap, Shell)
  * `100, 1000, 5000, 10000, 50000, 100000, 500000, 1000000, 2000000, 5000000, 10000000, 50000000, 100000000`

* For linear/special algorithms (Counting, Radix, Bucket)
  * Same as efficient algorithms above.

### 2.3 Data Patterns

Four canonical patterns are used to expose best/worst/average behavior.

| Pattern | Description | Use Case |
|---------|-------------|----------|
| `Random` | Uniform random integers | Typical average-case workload |
| `Sorted` | Already sorted ascending | Best case for Bubble/Insertion |
| `ReverseSorted` | Perfectly descending | Worst case for naive Insertion/Bubble |
| `NearlySorted` | Sorted with 5% random swaps | Real-world nearly-sorted data |

### 2.4 Pattern Test Sizes (by Complexity Class)

Each complexity class uses optimized sizes for pattern comparison:

| Complexity Class | Pattern Test Size | Rationale |
|------------------|-------------------|-----------|
| $O(n^2)$ | 50,000 | Larger sizes take too long |
| $O(n \log n)$ | 5,000,000 | Balances runtime and visibility |
| $O(n)$ | 50,000,000 | Linear algorithms can handle large inputs |

### 2.5 Timing & Environment

* Timing uses C's `clock()` (single-threaded CPU time).
* Each (algorithm, pattern, size) triple is run once.
* Large sizes (tens of millions) allocate hundreds of MB; ensure your machine has enough RAM.
* All results are verified for correctness using `qsort` as reference.

## 3. Benchmark Results

### 3.1 Key Findings Summary

#### $O(n^2)$ Algorithms (n = 50,000)

| Algorithm | Random | Sorted | Reverse Sorted | Nearly Sorted |
|-----------|--------|--------|----------------|---------------|
| Selection Sort | 1.33s | 0.79s | 1.18s | 0.79s |
| Bubble Sort | 2.86s | 0.00003s | 0.64s | 1.02s |
| Insertion Sort | 0.38s | 0.00004s | 0.77s | 0.05s |

**Key observations:**
- Bubble and Insertion sort achieve near-O(n) performance on sorted input (early-exit optimization).
- Selection sort has the same comparison count ($N(N-1)/2$) regardless of pattern, but execution times vary (0.79s–1.33s) due to differences in memory access patterns, branch prediction, and swap counts (0 swaps for sorted vs N/2 for reverse-sorted).
- Insertion sort is the fastest quadratic algorithm for random data.

#### $O(n \log n)$ Algorithms (n = 5,000,000)

| Algorithm | Random | Sorted | Reverse Sorted | Nearly Sorted |
|-----------|--------|--------|----------------|---------------|
| Merge Sort | 0.88s | 0.62s | 0.62s | 0.80s |
| Quick Sort | 0.56s | 0.21s | 0.24s | 0.30s |
| Heap Sort | 0.87s | 0.62s | 0.62s | 0.69s |
| Shell Sort | 1.20s | 0.11s | 0.17s | 1.02s |

**Key observations:**
- Quick sort (random pivot) is the fastest on most patterns, but Shell sort outperforms it on fully sorted data (0.11s vs 0.21s).
- Merge sort is the most consistent across patterns.
- Shell sort excels on sorted/reverse-sorted data. On nearly-sorted data, it performs better than random data (1.02s vs 1.20s) but is less efficient compared to Quick sort (0.30s).
- Shell sort uses the standard $n/2$ gap sequence, which is less optimal than advanced sequences (e.g., Ciura). This contributes to its relatively lower performance on random and nearly-sorted patterns.

#### $O(n)$ Algorithms (n = 50,000,000)

| Algorithm | Random | Sorted | Reverse Sorted | Nearly Sorted |
|-----------|--------|--------|----------------|---------------|
| Counting Sort | 0.39s | 0.16s | 0.12s | 0.38s |
| Radix Sort | 0.84s | 1.05s | 1.06s | 1.07s |
| Bucket Sort | 3.41s | 0.51s | 1.53s | 3.34s |

**Key observations:**
- Counting sort is the fastest when the value range is bounded.
- Radix sort is pattern-insensitive (consistent across all patterns).
- Bucket sort performance depends heavily on key distribution.

### 3.2 Size Benchmark Results (Random Pattern)

#### $O(n^2)$ Algorithms

| Size | Selection Sort | Bubble Sort | Insertion Sort |
|------|----------------|-------------|----------------|
| 100 | 0.000016s | 0.000014s | 0.000005s |
| 1,000 | 0.001432s | 0.000554s | 0.000171s |
| 5,000 | 0.027853s | 0.011530s | 0.003712s |
| 10,000 | 0.093561s | 0.044820s | 0.014697s |
| 20,000 | 0.288856s | 0.197010s | 0.058343s |
| 50,000 | 1.329131s | 2.862591s | 0.379543s |

#### $O(n^2)$ Best Case (Sorted Input)

| Size | Bubble Sort | Insertion Sort |
|------|-------------|----------------|
| 100 | 0.000001s | 0.000001s |
| 1,000 | 0.000000s | 0.000001s |
| 5,000 | 0.000003s | 0.000004s |
| 10,000 | 0.000006s | 0.000007s |
| 20,000 | 0.000011s | 0.000015s |
| 50,000 | 0.000027s | 0.000036s |

#### $O(n\log n)$ Algorithms

| Size | Merge Sort | Quick Sort | Heap Sort | Shell Sort |
|------|------------|------------|-----------|------------|
| 100 | 0.000020s | 0.000007s | 0.000007s | 0.000008s |
| 1,000 | 0.000180s | 0.000077s | 0.000055s | 0.000081s |
| 10,000 | 0.001439s | 0.000754s | 0.000716s | 0.001135s |
| 100,000 | 0.015560s | 0.009180s | 0.009909s | 0.015033s |
| 1,000,000 | 0.166311s | 0.104097s | 0.137347s | 0.198515s |
| 10,000,000 | 1.798379s | 1.185367s | 2.387424s | 2.697776s |
| 100,000,000 | 19.056227s | 15.055776s | 87.834218s | 34.960803s |

#### $O(n)$ Algorithms

| Size | Counting Sort | Radix Sort | Bucket Sort |
|------|---------------|------------|-------------|
| 100 | 0.001196s | 0.000006s | 0.000025s |
| 1,000 | 0.001064s | 0.000028s | 0.000086s |
| 10,000 | 0.000657s | 0.000171s | 0.000394s |
| 100,000 | 0.001356s | 0.001853s | 0.005591s |
| 1,000,000 | 0.006071s | 0.017115s | 0.076014s |
| 10,000,000 | 0.066816s | 0.171463s | 0.765687s |
| 100,000,000 | 0.863923s | 1.728238s | 6.764019s |

### 3.3 Best/Worst Case Summary

| Algorithm | Best Input | Worst Input | Notes |
|-----------|------------|-------------|-------|
| Selection | Any | Any | Always scans full unsorted portion |
| Bubble | Sorted | Random/Reverse Sorted | Early-exit on sorted input |
| Insertion | Sorted/Nearly Sorted | Reverse Sorted | Minimal shifts on sorted input |
| Merge | Any | Any | Deterministic work regardless of pattern |
| Quick (random pivot) | Random | Rare (bad pivot sequence) / Many duplicates | Random pivot avoids worst case; without 3-way partition, many duplicates can degrade to $O(n^2)$ |
| Heap | Any | Any | Cache-unfriendly but consistent |
| Shell | Sorted | Random | Performance highly dependent on gap sequence; standard $n/2$ sequence is suboptimal |
| Counting | Small range | Large range | O(n + k) where k is range |
| Radix | Any | Many digits | O(d * n) where d is digit count |
| Bucket | Uniform dist. | Skewed dist. | Depends on key distribution |

## 4. Repository Layout

```text
sorting-c-experimentorium/
├── include/
│   ├── benchmark.h      # benchmark driver API
│   ├── data_generator.h # test data patterns
│   ├── sorts.h          # sort declarations
│   └── visualizer.h     # gnuplot helpers
├── src/
│   ├── benchmark.c      # size/pattern sweep orchestration
│   ├── data_generator.c # pattern-based array generation
│   ├── visualizer.c     # gnuplot wrapper
│   └── sorts/
│       ├── bubble.c
│       ├── bucket.c
│       ├── counting.c
│       ├── heap.c
│       ├── insertion.c
│       ├── merge.c
│       ├── quick.c
│       ├── radix.c
│       ├── selection.c
│       └── shell.c
├── bin/                 # built benchmark binary
├── obj/                 # object files
├── results/             # CSV + PNG outputs
├── main.c               # CLI entry
├── Makefile
└── README.md
```

## 5. Build & Run

Build:

```bash
make
```

Run the full benchmark (including large sizes up to 100M where applicable):

```bash
./bin/benchmark
```

On macOS, install gnuplot first:

```bash
brew install gnuplot
```

## 6. Reading the Figures

### 6.1 Basic Quadratic Sorts

#### 6.1.1 `results/1_basic_sorts_log.png`

![Basic sorts (log)](results/1_basic_sorts_log.png)

This figure plots Selection, Bubble, and Insertion sort on a log-scale y-axis.
Because the vertical axis is logarithmic, the relative growth rates are easier to compare even when times diverge strongly at larger `n`.

* Bubble and Insertion typically track each other but may diverge depending on early-stopping optimizations.
* Selection sort often shows a flatter but consistently slow behavior because it always scans the unsorted tail fully.

#### 6.1.2 `results/1_basic_sorts_linear.png`

![Basic sorts (linear)](results/1_basic_sorts_linear.png)

On a linear y-axis, $O(n^2)$ behavior becomes much more dramatic.
Past a certain input size, all three algorithms quickly become impractical.

#### 6.1.3 `results/1_basic_sorts_best_linear.png`

![Basic sorts best-case (linear)](results/1_basic_sorts_best_linear.png)

This figure isolates the best-case scenario on already sorted input.

* Insertion sort on sorted input performs one comparison per element, giving near-linear performance.
* Bubble sort with early-exit optimization degenerates toward $O(n)$ when no swaps are needed.

### 6.2 Efficient Comparison Sorts

#### 6.2.1 `results/2_efficient_sorts_log.png`

![Efficient sorts (log)](results/2_efficient_sorts_log.png)

This log-scale figure compares Merge, Quick (random pivot), Heap, and Shell sort.

* Merge sort gives a very regular curve because its work is deterministic.
* Random-pivot quicksort tends to sit close to merge sort in the average case.
* Heap sort's constant factor is larger due to cache-unfriendly memory access.
* Shell sort uses the standard $n/2$ gap sequence, which is less efficient than advanced sequences (e.g., Ciura, Sedgewick). This results in relatively lower performance, especially on random and nearly-sorted data.

#### 6.2.2 `results/2_efficient_sorts_linear.png`

![Efficient sorts (linear)](results/2_efficient_sorts_linear.png)

On a linear scale, the slope of each $O(n \log n)$ algorithm becomes clearer.

### 6.3 Special / Non-comparison Sorts

#### 6.3.1 `results/3_special_sorts_log.png`

![Special sorts (log)](results/3_special_sorts_log.png)

This chart shows Counting sort, Radix sort, and Bucket sort on a log scale.
These algorithms are roughly linear when the key range and base are fixed.

#### 6.3.2 `results/3_special_sorts_linear.png`

![Special sorts (linear)](results/3_special_sorts_linear.png)

On the linear scale, their almost-linear behavior becomes visually obvious.

* Counting sort is extremely fast when the value range is bounded.
* Radix sort trades extra passes for independence from numeric magnitude.
* Bucket sort's performance depends on how evenly keys are distributed.

### 6.4 Pattern Comparison Plots

#### 6.4.1 `results/pattern_comparison_quadratic.png`

![Pattern comparison (quadratic)](results/pattern_comparison_quadratic.png)

This figure compares patterns for the quadratic algorithms:

* Huge penalty for `ReverseSorted` on insertion/bubble-style algorithms.
* Dramatic speedup for `Sorted` (best case).
* `NearlySorted` behavior is closer to the best case for insertion sort.

#### 6.4.2 `results/pattern_comparison_efficient.png`

![Pattern comparison (efficient)](results/pattern_comparison_efficient.png)

For $O(n \log n)$ algorithms, the gap between patterns is usually smaller:

* Merge sort is nearly insensitive to input order.
* Random-pivot quicksort handles most patterns gracefully.
* Heap and Shell sort show moderate differences.

#### 6.4.3 `results/pattern_comparison_special.png`

![Pattern comparison (special)](results/pattern_comparison_special.png)

For Counting/Radix/Bucket, pattern differences are often minimal.
Key range and distribution matter more than initial ordering.
