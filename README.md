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
| $O(n^2)$ | 100,000 | Larger sizes take too long |
| $O(n \log n)$ | 5,000,000 | Balances runtime and visibility |
| $O(n)$ | 50,000,000 | Linear algorithms can handle large inputs |

### 2.5 Timing & Environment

* Timing uses `clock_gettime(CLOCK_MONOTONIC)` for nanosecond-precision wall-clock measurement.
* Each (algorithm, pattern, size) triple is run once.
* Large sizes (tens of millions) allocate hundreds of MB; ensure your machine has enough RAM.
* All results are verified for correctness using `qsort` as reference.
* Swap and comparison counts are tracked for detailed algorithmic analysis.

## 3. Benchmark Results

### 3.1 Key Findings Summary

#### $O(n^2)$ Algorithms (n = 100,000)

Time in seconds (s):

| Algorithm | Random | Sorted | Reverse Sorted | Nearly Sorted |
|-----------|--------|--------|----------------|---------------|
| Selection Sort | `2.47` | `1.74` | `2.25` | `1.79` |
| Bubble Sort | `6.63` | `0.00003` | `1.49` | `2.30` |
| Insertion Sort | `0.88` | `0.00004` | `1.69` | `0.11` |

**Key observations:**
- Bubble and Insertion sort achieve near-O(n) performance on sorted input (early-exit optimization).
- Selection sort has the same comparison count ($N(N-1)/2$) regardless of pattern, but execution times vary (`1.74`–`2.47`) due to differences in memory access patterns, branch prediction, and swap counts (0 swaps for sorted vs N/2 for reverse-sorted).
- Insertion sort is the fastest quadratic algorithm for random data.

#### $O(n \log n)$ Algorithms (n = 5,000,000)

| Algorithm | Random | Sorted | Reverse Sorted | Nearly Sorted |
|-----------|--------|--------|----------------|---------------|
| Merge Sort | `0.53` | `0.36` | `0.36` | `0.46` |
| Quick Sort | `0.33` | `0.12` | `0.13` | `0.18` |
| Heap Sort | `0.52` | `0.35` | `0.36` | `0.44` |
| Shell Sort | `0.69` | `0.06` | `0.10` | `0.58` |

**Key observations:**
- Quick sort (random pivot) is the fastest on most patterns, but Shell sort outperforms it on fully sorted data (`0.06` vs `0.12`).
- Merge sort is the most consistent across patterns.
- Shell sort excels on sorted/reverse-sorted data. On nearly-sorted data, it performs better than random data (`0.58` vs `0.69`) but is less efficient compared to Quick sort (`0.18`).
- Shell sort uses the standard $n/2$ gap sequence, which is less optimal than advanced sequences (e.g., Ciura). This contributes to its relatively lower performance on random and nearly-sorted patterns.

#### $O(n)$ Algorithms (n = 50,000,000)

| Algorithm | Random | Sorted | Reverse Sorted | Nearly Sorted |
|-----------|--------|--------|----------------|---------------|
| Counting Sort | `0.31` | `0.09` | `0.07` | `0.28` |
| Radix Sort | `0.50` | `0.71` | `0.65` | `0.67` |

**Key observations:**
- Counting sort is the fastest when the value range is bounded.
- Radix sort is relatively pattern-insensitive but shows slightly slower performance on sorted/reverse-sorted data due to memory access patterns.

### 3.2 Size Benchmark Results (Random Pattern)

Time in seconds (s):

#### $O(n^2)$ Algorithms

| Size | Selection Sort | Bubble Sort | Insertion Sort |
|------|----------------|-------------|----------------|
| 100 | `0.000016` | `0.000009` | `0.000002` |
| 1,000 | `0.001704` | `0.000323` | `0.000121` |
| 5,000 | `0.051362` | `0.006096` | `0.002093` |
| 10,000 | `0.052629` | `0.024535` | `0.008940` |
| 20,000 | `0.186399` | `0.096269` | `0.033596` |
| 100,000 | `2.452065` | `6.591625` | `0.849394` |

#### $O(n^2)$ Best Case (Sorted Input)

| Size | Bubble Sort | Insertion Sort |
|------|-------------|----------------|
| 100 | `0.000000` | `0.000000` |
| 1,000 | `0.000000` | `0.000001` |
| 5,000 | `0.000001` | `0.000003` |
| 10,000 | `0.000004` | `0.000006` |
| 20,000 | `0.000009` | `0.000017` |
| 100,000 | `0.000037` | `0.000056` |

#### $O(n\log n)$ Algorithms

| Size | Merge Sort | Quick Sort | Heap Sort | Shell Sort |
|------|------------|------------|-----------|------------|
| 100 | `0.000012` | `0.000004` | `0.000003` | `0.000017` |
| 1,000 | `0.000104` | `0.000051` | `0.000033` | `0.000074` |
| 10,000 | `0.000832` | `0.000438` | `0.000416` | `0.000711` |
| 100,000 | `0.009211` | `0.005350` | `0.005989` | `0.010483` |
| 1,000,000 | `0.096160` | `0.060919` | `0.078841` | `0.124613` |
| 10,000,000 | `1.024658` | `0.675500` | `1.258083` | `1.469666` |
| 100,000,000 | `11.087366` | `8.629399` | `39.067889` | `19.842090` |

#### $O(n)$ Algorithms

| Size | Counting Sort | Radix Sort |
|------|---------------|------------|
| 100 | `0.000599` | `0.000003` |
| 1,000 | `0.000557` | `0.000018` |
| 10,000 | `0.000386` | `0.000115` |
| 100,000 | `0.000699` | `0.001030` |
| 1,000,000 | `0.003587` | `0.010058` |
| 10,000,000 | `0.053088` | `0.100978` |
| 100,000,000 | `0.591969` | `1.057056` |

### 3.3 Operation Count Analysis (Bubble Sort)

The `bubble_sort_stats()` function tracks comparisons and swaps for detailed algorithmic analysis.

#### Actual Counts (n = 10,000)

Time in seconds (s):

| Pattern | Comparisons | Swaps | Time |
|---------|-------------|-------|------|
| Random | `49,967,034` | `25,063,713` | `0.041` |
| Sorted | `9,999` | `0` | `0.000003` |
| Reverse Sorted | `49,995,000` | `49,995,000` | `0.020` |
| Nearly Sorted | `49,912,379` | `3,031,830` | `0.021` |

#### Scaling with Size (Random Pattern)

| Size | Comparisons | Swaps | Time |
|------|-------------|-------|------|
| 100 | `4,650` | `2,554` | `0.000016` |
| 1,000 | `498,275` | `245,886` | `0.000955` |
| 5,000 | `12,495,609` | `6,205,093` | `0.020` |
| 10,000 | `49,967,034` | `25,063,713` | `0.041` |
| 50,000 | `1,249,970,149` | `626,215,467` | `1.69` |

**Key observations:**
- **Sorted input**: Only $n-1$ comparisons and 0 swaps (early termination after first pass).
- **Reverse-sorted input**: Exactly $\frac{n(n-1)}{2}$ comparisons AND swaps (theoretical worst case).
- **Random input**: ~ $\frac{n(n-1)}{2}$ comparisons, ~ $\frac{n(n-1)}{4}$ swaps (half elements need swapping on average).
- Comparison count directly correlates with execution time.

### 3.4 Best/Worst Case Summary

| Algorithm | Best Input | Worst Input | Notes |
|-----------|------------|-------------|-------|
| Selection | Any | Any | Always scans full unsorted portion |
| Bubble | Sorted | Random/Reverse Sorted | Early-exit on sorted input |
| Insertion | Sorted/Nearly Sorted | Reverse Sorted | Minimal shifts on sorted input |
| Merge | Any | Any | Deterministic work regardless of pattern |
| Quick (random pivot) | Random | Rare (bad pivot sequence) / Many duplicates | Random pivot avoids worst case; without 3-way partition, many duplicates can degrade to $O(n^2)$ |
| Heap | Any | Any | Cache-unfriendly but consistent |
| Shell | Sorted | Random | Performance highly dependent on gap sequence; standard $n/2$ sequence is suboptimal |
| Counting | Small range | Large range | $O(n + k)$ where k is range |
| Radix | Any | Many digits | $O(d * n)$ where d is digit count |
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

Run only the stats benchmark (comparison/swap counts for Bubble Sort):

```bash
./bin/benchmark --stats-only
```

Regenerate plots from existing CSV data:

```bash
./bin/benchmark --plot-only
```

Run without extended sizes (faster, max 1M elements):

```bash
./bin/benchmark --no-large-sizes
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
Because the vertical axis is logarithmic, the relative growth rates are easier to compare even when times diverge strongly at larger $n$.

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
