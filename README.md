# Sorting Algorithm Performance Benchmark Report

## 1. Goals

- Implement a clean, idiomatic C version of 10 sorting algorithms behind a common interface.
- Benchmark them under controlled conditions (same machine, same generator, same timing API).
- Visualize how time complexity behaves in practice:
  - $O(n^)$ vs $O(n \log n)$ vs "linear-ish" ($O(n)$ style) non-comparison sorts.
  - Sensitivity to input order: random / already sorted / reverse sorted / nearly sorted.

## 2. Algorithms & Input Models

### 2.1 Algorithms Implemented

All algorithms share the same `void sort(int *arr, int n)`-style API behind `sorts.h`, and are selected by name in the benchmark.

* Quadratic comparison sorts ($O(n^2)$ in the general case):

  * Selection sort (`selection.c`)
  * Bubble sort (`bubble.c`)
  * Insertion sort (`insertion.c`)

* Efficient comparison sorts ($O(n \log n)$ on average / worst):

  * Merge sort (`merge.c`)
  * Quick sort with random pivot (`quick.c`)
  * Heap sort (`heap.c`)
  * Shell sort with Ciura-style gaps (`shell.c`)

* Special / non-comparison-based sorts (roughly linear in range/keys):

  * Counting sort (`counting.c`)
  * Radix sort (LSD, base 10) (`radix.c`)
  * Bucket sort with 10 buckets (`bucket.c`)


### 2.2 Input Sizes

Different size grids are used so that each family is pushed to the regime where its complexity is visible.

* For quadratic algorithms (Selection, Bubble, Insertion – random data):

  * `100, 1000, 5000, 10000, 20000, 30000, 40000, 50000`
* For quadratic best-case (Bubble / Insertion on sorted input):

  * Same sizes as above.
* For efficient and special algorithms (Merge, Quick, Heap, Shell, Counting, Radix, Bucket):

  * `100, 1000, 5000, 10000, 50000, 100000, 500000,`
  * `1000000, 2000000, 5000000, 10000 000, 50000000, 100000000`

### 2.3 Data Patterns

Four canonical patterns are used to expose best/worst/average behavior.

* `Random`
  Independent uniform random integers in the configured range.
  Represents a typical average-case workload.

* `Sorted`
  Already sorted in ascending order.
  Best case for insertion-style and early-stopping bubble sort.

* `ReverseSorted`
  Perfectly descending order.
  Worst case for naive insertion / bubble, and some pivot choices in quicksort.

* `NearlySorted`
  Start from sorted data, then randomly swap about 5% of the elements.
  Models “real-world” arrays that are almost sorted but slightly perturbed.

### 2.4 Timing & Environment

* Timing uses C’s `clock()` (single-threaded CPU time).
* Each (algorithm, pattern, size) triple is currently run once; you can extend this to multiple runs and averages.
* Large sizes (≥ tens of millions) allocate hundreds of MB; ensure your machine has enough RAM and avoid running heavy background tasks.

## 3. Repository Layout

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

## 4. Build & Run

* Build:

```bash
make
```

* Run the full benchmark (including large sizes up to 100M where applicable):

```bash
./bin/benchmark
```
On macOS, you may want to install gnuplot first:

```bash
brew install gnuplot
```

## 5. Reading the Figures

### 5.1 Basic Quadratic Sorts

#### 5.1.1 `results/1_basic_sorts_log.png`

![Basic sorts (log)](results/1_basic_sorts_log.png)

This figure plots Selection, Bubble, and Insertion sort on a log-scale y-axis.
Because the vertical axis is logarithmic, the relative growth rates are easier to compare even when times diverge strongly at larger `n`.

* Bubble and Insertion typically track each other but may diverge depending on early-stopping optimizations.
* Selection sort often shows a flatter but consistently slow behavior because it always scans the unsorted tail fully and performs relatively few swaps.

#### 5.1.2 `results/1_basic_sorts_linear.png`

![Basic sorts (linear)](results/1_basic_sorts_linear.png)

On a linear y-axis, $O(n^2)$ behavior becomes much more dramatic.
Past a certain input size, all three algorithms quickly become impractical, and the curve shape matches the quadratic reference line.

#### 5.1.3 `results/1_basic_sorts_best_linear.png`

![Basic sorts best-case (linear)](results/1_basic_sorts_best_linear.png)

This figure isolates the best-case scenario on already sorted input, focusing on algorithms that can exploit it (Bubble and Insertion sorts).

* Insertion sort on sorted input effectively performs one comparison per element, giving a near-linear profile in practice.
* Bubble sort with early-exit optimization also degenerates toward $O(n)$ when no swaps are needed.

### 5.2 Efficient Comparison Sorts

#### 5.2.1 `results/2_efficient_sorts_log.png`

![Efficient sorts (log)](results/2_efficient_sorts_log.png)

This log-scale figure compares Merge, Quick (random pivot), Heap, and Shell sort.
Here, the goal is to see how close they are asymptotically and how their constants differ.

* Merge sort often gives a very regular curve because its work is deterministic across inputs.
* Random-pivot quicksort can fluctuate slightly but tends to sit close to merge sort in the average case.
* Heap sort’s constant factor is sometimes larger due to relatively cache-unfriendly memory access.
* Shell sort’s behavior depends strongly on the gap sequence; with Ciura gaps, it can be competitive for practical ranges.

#### 5.2.2 `results/2_efficient_sorts_linear.png`

![Efficient sorts (linear)](results/2_efficient_sorts_linear.png)

On a linear scale, the slope of each $O(n \log n)$ algorithm becomes clearer.
The exact ordering between Merge, Quick, Heap, and Shell for each size range is often determined more by constants and memory access patterns than by asymptotic complexity.

### 5.3 Special / Non-comparison Sorts

#### 5.3.1 `results/3_special_sorts_log.png`

![Special sorts (log)](results/3_special_sorts_log.png)

This chart shows Counting sort, Radix sort, and Bucket sort on a log scale.
These algorithms are roughly linear in the input size when the key range and base are fixed.

#### 5.3.2 `results/3_special_sorts_linear.png`

![Special sorts (linear)](results/3_special_sorts_linear.png)

On the linear scale, their almost-linear behavior becomes visually obvious:
for large enough `n`, these curves should separate clearly from the $O(n \log n)$ group.

* Counting sort is extremely fast when the value range is tightly bounded.
* Radix sort trades extra passes over the data for independence from numeric magnitude (within the chosen representation).
* Bucket sort’s performance depends heavily on how evenly keys are distributed among buckets.

### 5.4 Pattern Comparison Plots

The three pattern comparison plots show how input ordering affects each algorithm family.

#### 5.4.1 `results/pattern_comparison_quadratic.png`

![Pattern comparison (quadratic)](results/pattern_comparison_quadratic.png)

This figure compares `Random`, `Sorted`, `ReverseSorted`, and `NearlySorted` for the quadratic algorithms. You should see:

* Huge penalty for `ReverseSorted` on insertion/bubble-style algorithms.
* Dramatic speedup for `Sorted` (best case).
* `NearlySorted` behavior sitting between the extremes, often much closer to the best case for insertion sort.

#### 5.4.2 `results/pattern_comparison_efficient.png`

![Pattern comparison (efficient)](results/pattern_comparison_efficient.png)

For $O(n \log n)$ algorithms, the gap between patterns is usually smaller:

* Merge sort is nearly insensitive to input order.
* Random-pivot quicksort handles most patterns gracefully but still can show some variance.
* Heap and Shell sort may show moderate differences, but not as extreme as the quadratic family.

#### 5.4.3 `results/pattern_comparison_special.png`

![Pattern comparison (special)](results/pattern_comparison_special.png)

For Counting/Radix/Bucket, pattern differences are often minimal, as long as the value distribution assumptions are satisfied.
This shows that for these algorithms, key range and distribution matter more than initial ordering.
