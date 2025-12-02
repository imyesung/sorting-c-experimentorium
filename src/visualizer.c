#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/visualizer.h"

#define ARRAY_SIZE(arr) (int)(sizeof(arr) / sizeof((arr)[0]))
#define MAX_LINE_LENGTH 512
#define MAX_NAME_LENGTH 64

typedef enum {
    TREND_NONE,
    TREND_N,
    TREND_NLOGN,
    TREND_N2
} TrendLine;

typedef struct {
    char name[MAX_NAME_LENGTH];
    double times[4];
    bool seen[4];
} PatternEntry;

static void plot_group(const char *output_file, const char *title,
                       const char *algorithms[], int count,
                       const char *pattern_filter,
                       bool use_logscale, TrendLine trend) {
    FILE *csv = fopen("results/size_benchmark.csv", "r");
    if (csv == NULL) {
        printf("Error: Could not open size_benchmark.csv\n");
        return;
    }

    FILE **temp_files = (FILE **)calloc(count, sizeof(FILE *));
    char (*temp_paths)[256] = calloc((size_t)count, sizeof(*temp_paths));
    int *data_counts = (int *)calloc(count, sizeof(int));  // Track data points per algorithm
    if (temp_files == NULL || temp_paths == NULL || data_counts == NULL) {
        printf("Error: Could not allocate temporary buffers for plotting\n");
        free(temp_files);
        free(temp_paths);
        free(data_counts);
        fclose(csv);
        return;
    }

    for (int i = 0; i < count; i++) {
        snprintf(temp_paths[i], sizeof(temp_paths[i]), "results/.temp_%s_size.dat", algorithms[i]);
        temp_files[i] = fopen(temp_paths[i], "w");
        if (temp_files[i] == NULL) {
            printf("Error: Could not create %s\n", temp_paths[i]);
            for (int j = 0; j <= i; j++) {
                if (temp_files[j] != NULL) {
                    fclose(temp_files[j]);
                    remove(temp_paths[j]);
                }
            }
            free(temp_files);
            free(temp_paths);
            fclose(csv);
            return;
        }
    }

    char line[MAX_LINE_LENGTH];
    if (fgets(line, sizeof(line), csv) == NULL) {
        printf("Error: size_benchmark.csv is empty\n");
        goto cleanup;
    }

    double reference_size = 0.0;
    double reference_time = 0.0;

    while (fgets(line, sizeof(line), csv)) {
        char algo[MAX_NAME_LENGTH];
        char pattern[MAX_NAME_LENGTH];
        int size;
        double time;

        if (sscanf(line, "%63[^,],%63[^,],%d,%lf", algo, pattern, &size, &time) != 4) {
            continue;
        }

        if (pattern_filter != NULL && strcmp(pattern, pattern_filter) != 0) {
            continue;
        }

        for (int i = 0; i < count; i++) {
            if (strcmp(algo, algorithms[i]) == 0) {
                fprintf(temp_files[i], "%d,%lf\n", size, time);
                data_counts[i]++;  // Count data points
                if (i == 0) {
                    if (reference_size <= 0.0 || size < reference_size) {
                        reference_size = (double)size;
                        reference_time = time;
                    }
                }
                break;
            }
        }
    }

cleanup:
    fclose(csv);
    for (int i = 0; i < count; i++) {
        if (temp_files[i] != NULL) {
            fclose(temp_files[i]);
        }
    }

    FILE *gp = popen("gnuplot", "w");
    if (gp == NULL) {
        printf("Error: Could not open gnuplot\n");
        for (int i = 0; i < count; i++) {
            remove(temp_paths[i]);
        }
        free(temp_files);
        free(temp_paths);
        return;
    }

    fprintf(gp, "set terminal png size 1800,1100 font 'Arial,14'\n");
    fprintf(gp, "set output 'results/%s'\n", output_file);
    fprintf(gp, "set title '%s' font 'Arial,20' enhanced\n", title);
    if (use_logscale) {
        fprintf(gp, "set xlabel 'Input Size n (log scale)' font 'Arial,16' offset 0,-0.5\n");
        fprintf(gp, "set ylabel 'Execution Time (seconds, log scale)' font 'Arial,16' offset -1,0\n");
        fprintf(gp, "set logscale xy\n");
        fprintf(gp, "set autoscale\n");
    } else {
        fprintf(gp, "set xlabel 'Input Size n' font 'Arial,16' offset 0,-0.5\n");
        fprintf(gp, "set ylabel 'Execution Time (seconds)' font 'Arial,16' offset -1,0\n");
        fprintf(gp, "unset logscale\n");
        fprintf(gp, "set autoscale\n");
        fprintf(gp, "set yrange [0:*]\n");
    }
    
    // Enhanced grid styling
    fprintf(gp, "set grid xtics ytics mxtics mytics lc rgb '#dddddd' lw 1 lt 1\n");
    fprintf(gp, "set grid mxtics mytics lc rgb '#eeeeee' lw 0.5 lt 1\n");
    fprintf(gp, "set mxtics 5\n");
    fprintf(gp, "set mytics 5\n");
    
    // Key/legend styling with data point counts
    fprintf(gp, "set key outside right top box opaque font 'Arial,13' spacing 1.5\n");
    fprintf(gp, "set key title 'Algorithms (n = data points)' font 'Arial,12'\n");
    fprintf(gp, "set datafile separator ','\n");
    
    // Border styling
    fprintf(gp, "set border 3 lw 2\n");
    fprintf(gp, "set tics font 'Arial,12'\n");
    fprintf(gp, "set format x '%%.0s%%c'\n");  // Format large numbers (1M, 10M, etc.)
    
    // Line styles for each algorithm (distinct colors and point styles)
    fprintf(gp, "set style line 1 lc rgb '#e41a1c' lt 1 lw 3 pt 7 ps 1.5\n");  // Red, circle
    fprintf(gp, "set style line 2 lc rgb '#377eb8' lt 1 lw 3 pt 5 ps 1.5\n");  // Blue, square
    fprintf(gp, "set style line 3 lc rgb '#4daf4a' lt 1 lw 3 pt 9 ps 1.5\n");  // Green, triangle
    fprintf(gp, "set style line 4 lc rgb '#984ea3' lt 1 lw 3 pt 13 ps 1.5\n"); // Purple, diamond
    fprintf(gp, "set style line 5 lc rgb '#ff7f00' lt 1 lw 3 pt 11 ps 1.5\n"); // Orange, inverted triangle
    fprintf(gp, "set style line 100 lc rgb '#888888' lt 1 lw 2 dt 3\n");       // Trend line (dashed gray)

    double trend_factor = 0.0;
    const char *trend_title = NULL;
    if (reference_size > 0.0 && reference_time > 0.0) {
        if (trend == TREND_N) {
            trend_factor = reference_time / reference_size;
            trend_title = "Theoretical guide (O(n))";
        } else if (trend == TREND_N2) {
            trend_factor = reference_time / (reference_size * reference_size);
            trend_title = "Theoretical guide (O(n^2))";
        } else if (trend == TREND_NLOGN) {
            double log_term = log(reference_size);
            if (log_term > 0.0) {
                trend_factor = reference_time / (reference_size * log_term);
                trend_title = "Theoretical guide (O(n log n))";
            }
        }
    }

    if (trend_factor <= 0.0) {
        trend = TREND_NONE;
    }

    if (trend == TREND_N) {
        fprintf(gp, "f(x) = %.15e * x\n", trend_factor);
    } else if (trend == TREND_NLOGN) {
        fprintf(gp, "f(x) = %.15e * x * log(x)\n", trend_factor);
    } else if (trend == TREND_N2) {
        fprintf(gp, "f(x) = %.15e * x * x\n", trend_factor);
    }

    fprintf(gp, "plot ");
    for (int i = 0; i < count; i++) {
        fprintf(gp, "'%s' using 1:2 with linespoints ls %d title '%s (n=%d)'",
                temp_paths[i], i + 1, algorithms[i], data_counts[i]);
        if (i < count - 1 || trend != TREND_NONE) {
            fprintf(gp, ", \\\n     ");
        }
    }

    if (trend != TREND_NONE && trend_title != NULL) {
        fprintf(gp, "f(x) with lines ls 100 title '%s'", trend_title);
    }
    fprintf(gp, "\n");
    fflush(gp);
    pclose(gp);

    for (int i = 0; i < count; i++) {
        remove(temp_paths[i]);
    }

    free(temp_files);
    free(temp_paths);
    free(data_counts);
}

static int pattern_index(const char *pattern) {
    static const char *patterns[] = {
        "Random",
        "Sorted",
        "ReverseSorted",
        "NearlySorted"
    };

    for (int i = 0; i < 4; i++) {
        if (strcmp(pattern, patterns[i]) == 0) {
            return i;
        }
    }
    return -1;
}

static PatternEntry *find_pattern_entry(PatternEntry *entries, int entry_count,
                                        const char *name) {
    for (int i = 0; i < entry_count; i++) {
        if (strcmp(entries[i].name, name) == 0) {
            return &entries[i];
        }
    }
    return NULL;
}

static void plot_pattern_subset(const char *output_file, const char *title,
                                PatternEntry *entries, int entry_count,
                                const char *const *names, int name_count) {
    const char *temp_path = "results/.temp_pattern_subset.dat";
    FILE *temp = fopen(temp_path, "w");
    if (temp == NULL) {
        printf("Error: Could not create %s\n", temp_path);
        return;
    }

    fprintf(temp, "Algorithm,Random,Sorted,ReverseSorted,NearlySorted\n");
    int rows_written = 0;

    for (int i = 0; i < name_count; i++) {
        PatternEntry *entry = find_pattern_entry(entries, entry_count, names[i]);
        if (entry == NULL) {
            continue;
        }

        fprintf(temp, "%s", entry->name);
        for (int p = 0; p < 4; p++) {
            if (entry->seen[p]) {
                fprintf(temp, ",%.6f", entry->times[p]);
            } else {
                fprintf(temp, ",");
            }
        }
        fprintf(temp, "\n");
        rows_written++;
    }

    fclose(temp);

    if (rows_written == 0) {
        printf("Warning: No data available for pattern subset %s\n", title);
        remove(temp_path);
        return;
    }

    FILE *gp = popen("gnuplot", "w");
    if (gp == NULL) {
        printf("Error: Could not open gnuplot\n");
        remove(temp_path);
        return;
    }

    fprintf(gp, "set terminal png size 1800,1200 font 'Arial,14'\n");
    fprintf(gp, "set output 'results/%s'\n", output_file);
    fprintf(gp, "set title '%s' font 'Arial,20' enhanced\n", title);
    fprintf(gp, "set xlabel 'Algorithm' font 'Arial,16' offset 0,-1\n");
    fprintf(gp, "set ylabel 'Execution Time (seconds)' font 'Arial,16' offset -1,0\n");
    fprintf(gp, "set style data histogram\n");
    fprintf(gp, "set style histogram clustered gap 2\n");
    fprintf(gp, "set style fill solid 0.8 border -1\n");
    fprintf(gp, "set boxwidth 0.9\n");
    fprintf(gp, "set xtics rotate by -30 font 'Arial,13' offset 0,-0.5\n");
    fprintf(gp, "set ytics font 'Arial,12'\n");
    fprintf(gp, "set grid ytics lc rgb '#dddddd' lw 1\n");
    fprintf(gp, "set key outside right top box opaque font 'Arial,14' spacing 1.5\n");
    fprintf(gp, "set key title 'Input Patterns' font 'Arial,13'\n");
    fprintf(gp, "set border 3 lw 2\n");
    fprintf(gp, "set datafile separator ','\n");
    fprintf(gp, "set yrange [0:*]\n");
    
    // Distinct colors for each pattern
    fprintf(gp, "set style line 1 lc rgb '#e41a1c'\n");  // Random: Red
    fprintf(gp, "set style line 2 lc rgb '#377eb8'\n");  // Sorted: Blue
    fprintf(gp, "set style line 3 lc rgb '#4daf4a'\n");  // Reverse Sorted: Green
    fprintf(gp, "set style line 4 lc rgb '#984ea3'\n");  // Nearly Sorted: Purple

    fprintf(gp, "plot '%s' using 2:xtic(1) ls 1 title 'Random', \\\n", temp_path);
    fprintf(gp, "     '' using 3:xtic(1) ls 2 title 'Sorted', \\\n");
    fprintf(gp, "     '' using 4:xtic(1) ls 3 title 'Reverse Sorted', \\\n");
    fprintf(gp, "     '' using 5:xtic(1) ls 4 title 'Nearly Sorted'\n");

    fflush(gp);
    pclose(gp);

    remove(temp_path);
}

void plot_size_comparison(const char *output_file) {
    (void)output_file;

    const char *basic_sorts[] = {"SelectionSort", "BubbleSort", "InsertionSort"};
    plot_group("1_basic_sorts_log.png", "O(n²) Sorting Algorithms Performance (log scale)",
               basic_sorts, 3, "Random", true, TREND_N2);
    plot_group("1_basic_sorts_linear.png", "O(n²) Sorting Algorithms Performance (linear scale)",
               basic_sorts, 3, "Random", false, TREND_N2);

    const char *efficient_sorts[] = {"MergeSort", "QuickSort", "HeapSort"};
    plot_group("2_efficient_sorts_log.png", "O(n log n) Sorting Algorithms Performance (log scale)",
               efficient_sorts, 3, "Random", true, TREND_NLOGN);
    plot_group("2_efficient_sorts_linear.png", "O(n log n) Sorting Algorithms Performance (linear scale)",
               efficient_sorts, 3, "Random", false, TREND_NLOGN);

    const char *special_sorts[] = {"ShellSort", "CountingSort", "RadixSort", "BucketSort"};
    plot_group("3_special_sorts_log.png", "Special Sorting Algorithms Performance (log scale)",
               special_sorts, 4, "Random", true, TREND_NONE);
    plot_group("3_special_sorts_linear.png", "Special Sorting Algorithms Performance (linear scale)",
               special_sorts, 4, "Random", false, TREND_NONE);

    const char *basic_best_case[] = {"BubbleSort", "InsertionSort"};
    plot_group("1_basic_sorts_best_linear.png", "Near O(n) Best-Case Performance (sorted input)",
               basic_best_case, 2, "Sorted", false, TREND_N);
}

void plot_pattern_comparison(void) {
    FILE *csv = fopen("results/pattern_benchmark.csv", "r");
    if (csv == NULL) {
        printf("Error: Could not open pattern_benchmark.csv\n");
        return;
    }

    PatternEntry entries[32];
    int entry_count = 0;
    memset(entries, 0, sizeof(entries));

    char line[MAX_LINE_LENGTH];
    if (fgets(line, sizeof(line), csv) == NULL) {
        printf("Error: pattern_benchmark.csv is empty\n");
        fclose(csv);
        return;
    }

    while (fgets(line, sizeof(line), csv)) {
        char algo[MAX_NAME_LENGTH];
        char pattern[MAX_NAME_LENGTH];
        int size;
        double time;

        if (sscanf(line, "%63[^,],%63[^,],%d,%lf", algo, pattern, &size, &time) != 4) {
            continue;
        }

        int p_index = pattern_index(pattern);
        if (p_index < 0) {
            continue;
        }

        int entry_idx = -1;
        for (int i = 0; i < entry_count; i++) {
            if (strcmp(entries[i].name, algo) == 0) {
                entry_idx = i;
                break;
            }
        }

        if (entry_idx < 0 && entry_count < (int)ARRAY_SIZE(entries)) {
            entry_idx = entry_count++;
            strncpy(entries[entry_idx].name, algo, MAX_NAME_LENGTH - 1);
            entries[entry_idx].name[MAX_NAME_LENGTH - 1] = '\0';
        }

        if (entry_idx >= 0) {
            entries[entry_idx].times[p_index] = time;
            entries[entry_idx].seen[p_index] = true;
        }
    }

    fclose(csv);

    if (entry_count == 0) {
        printf("Warning: pattern_benchmark.csv has no data to plot\n");
        return;
    }

    const char *special_algorithms[] = {
        "CountingSort",
        "RadixSort",
        "BucketSort"
    };

    const char *quadratic_algorithms[] = {
        "SelectionSort",
        "BubbleSort",
        "InsertionSort"
    };

    const char *efficient_algorithms[] = {
        "MergeSort",
        "QuickSort",
        "HeapSort",
        "ShellSort"
    };

    plot_pattern_subset("pattern_comparison_quadratic.png",
                        "Quadratic Algorithms by Data Pattern",
                        entries, entry_count,
                        quadratic_algorithms,
                        ARRAY_SIZE(quadratic_algorithms));

    plot_pattern_subset("pattern_comparison_efficient.png",
                        "O(n log n) Algorithms by Data Pattern",
                        entries, entry_count,
                        efficient_algorithms,
                        ARRAY_SIZE(efficient_algorithms));

    plot_pattern_subset("pattern_comparison_special.png",
                        "Non-Comparison Algorithms by Data Pattern",
                        entries, entry_count,
                        special_algorithms,
                        ARRAY_SIZE(special_algorithms));
}

void generate_all_plots() {
    printf("\nGenerating visualization graphs...\n");

    printf("  Creating O(n²) algorithms graph...\n");
    printf("  Creating O(n log n) algorithms graph...\n");
    printf("  Creating special algorithms graph...\n");
    plot_size_comparison(NULL);

    printf("  Creating pattern comparison graphs (quadratic / n log n / special)...\n");
    plot_pattern_comparison();

    printf("Graph generation completed!\n");
    printf("Check results/ folder for PNG files:\n");
    printf("  - 1_basic_sorts_log.png / 1_basic_sorts_linear.png\n");
    printf("  - 1_basic_sorts_best_linear.png (Bubble, Insertion sorted input)\n");
    printf("  - 2_efficient_sorts_log.png / 2_efficient_sorts_linear.png\n");
    printf("  - 3_special_sorts_log.png / 3_special_sorts_linear.png\n");
    printf("  - pattern_comparison_quadratic.png (Selection, Bubble, Insertion)\n");
    printf("  - pattern_comparison_efficient.png (Merge, Quick, Heap, Shell)\n");
    printf("  - pattern_comparison_special.png (Counting, Radix, Bucket)\n");
}
