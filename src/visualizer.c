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
    if (temp_files == NULL || temp_paths == NULL) {
        printf("Error: Could not allocate temporary buffers for plotting\n");
        free(temp_files);
        free(temp_paths);
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

    fprintf(gp, "set terminal png size 1600,1000 font 'Arial,12'\n");
    fprintf(gp, "set output 'results/%s'\n", output_file);
    fprintf(gp, "set title '%s' font 'Arial,16'\n", title);
    if (use_logscale) {
        fprintf(gp, "set xlabel 'Input Size (log scale)' font 'Arial,14'\n");
        fprintf(gp, "set ylabel 'Execution Time (seconds, log scale)' font 'Arial,14'\n");
        fprintf(gp, "set logscale xy\n");
    } else {
        fprintf(gp, "set xlabel 'Input Size' font 'Arial,14'\n");
        fprintf(gp, "set ylabel 'Execution Time (seconds)' font 'Arial,14'\n");
        fprintf(gp, "unset logscale\n");
    }
    fprintf(gp, "set grid\n");
    fprintf(gp, "set key outside right top\n");
    fprintf(gp, "set datafile separator ','\n");

    double trend_factor = 0.0;
    const char *trend_title = NULL;
    if (reference_size > 0.0 && reference_time > 0.0) {
        if (trend == TREND_N) {
            trend_factor = reference_time / reference_size;
            trend_title = "O(n) reference";
        } else if (trend == TREND_N2) {
            trend_factor = reference_time / (reference_size * reference_size);
            trend_title = "O(n^2) reference";
        } else if (trend == TREND_NLOGN) {
            double log_term = log(reference_size);
            if (log_term > 0.0) {
                trend_factor = reference_time / (reference_size * log_term);
                trend_title = "O(n log n) reference";
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
        fprintf(gp, "'%s' using 1:2 with linespoints title '%s'",
                temp_paths[i], algorithms[i]);
        if (i < count - 1 || trend != TREND_NONE) {
            fprintf(gp, ", \\\n+     ");
        }
    }

    if (trend != TREND_NONE && trend_title != NULL) {
        fprintf(gp, "f(x) with lines title '%s' lw 2 dt 3", trend_title);
    }
    fprintf(gp, "\n");
    fflush(gp);
    pclose(gp);

    for (int i = 0; i < count; i++) {
        remove(temp_paths[i]);
    }

    free(temp_files);
    free(temp_paths);
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

void plot_pattern_comparison(const char *output_file) {
    FILE *csv = fopen("results/pattern_benchmark.csv", "r");
    if (csv == NULL) {
        printf("Error: Could not open pattern_benchmark.csv\n");
        return;
    }

    typedef struct {
        char name[MAX_NAME_LENGTH];
        double times[4];
        bool seen[4];
    } PatternEntry;

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

    const char *temp_path = "results/.temp_pattern_histogram.dat";
    FILE *temp = fopen(temp_path, "w");
    if (temp == NULL) {
        printf("Error: Could not create %s\n", temp_path);
        return;
    }

    fprintf(temp, "Algorithm,Random,Sorted,ReverseSorted,NearlySorted\n");
    for (int i = 0; i < entry_count; i++) {
        fprintf(temp, "%s", entries[i].name);
        for (int p = 0; p < 4; p++) {
            if (entries[i].seen[p]) {
                fprintf(temp, ",%.6f", entries[i].times[p]);
            } else {
                fprintf(temp, ",");
            }
        }
        fprintf(temp, "\n");
    }

    fclose(temp);

    if (entry_count == 0) {
        printf("Warning: pattern_benchmark.csv has no data to plot\n");
        remove(temp_path);
        return;
    }

    FILE *gp = popen("gnuplot", "w");
    if (gp == NULL) {
        printf("Error: Could not open gnuplot\n");
        remove(temp_path);
        return;
    }

    fprintf(gp, "set terminal png size 1800,1200 font 'Arial,11'\n");
    fprintf(gp, "set output 'results/%s'\n", output_file);
    fprintf(gp, "set title 'Algorithm Performance by Data Pattern' font 'Arial,16'\n");
    fprintf(gp, "set xlabel 'Algorithm' font 'Arial,13'\n");
    fprintf(gp, "set ylabel 'Execution Time (seconds)' font 'Arial,13'\n");
    fprintf(gp, "set style data histogram\n");
    fprintf(gp, "set style histogram clustered gap 1\n");
    fprintf(gp, "set style fill solid border -1\n");
    fprintf(gp, "set boxwidth 0.9\n");
    fprintf(gp, "set xtics rotate by -45 font 'Arial,10'\n");
    fprintf(gp, "set grid ytics\n");
    fprintf(gp, "set key outside right top\n");
    fprintf(gp, "set datafile separator ','\n");

    fprintf(gp, "plot '%s' using 2:xtic(1) title 'Random', \\\n", temp_path);
    fprintf(gp, "     '' using 3:xtic(1) title 'Sorted', \\\n");
    fprintf(gp, "     '' using 4:xtic(1) title 'Reverse Sorted', \\\n");
    fprintf(gp, "     '' using 5:xtic(1) title 'Nearly Sorted'\n");

    fflush(gp);
    pclose(gp);

    remove(temp_path);
}

void generate_all_plots() {
    printf("\nGenerating visualization graphs...\n");

    printf("  Creating O(n²) algorithms graph...\n");
    printf("  Creating O(n log n) algorithms graph...\n");
    printf("  Creating special algorithms graph...\n");
    plot_size_comparison(NULL);

    printf("  Creating pattern comparison graph...\n");
    plot_pattern_comparison("pattern_comparison.png");

    printf("Graph generation completed!\n");
    printf("Check results/ folder for PNG files:\n");
    printf("  - 1_basic_sorts_log.png / 1_basic_sorts_linear.png\n");
    printf("  - 1_basic_sorts_best_linear.png (Bubble, Insertion sorted input)\n");
    printf("  - 2_efficient_sorts_log.png / 2_efficient_sorts_linear.png\n");
    printf("  - 3_special_sorts_log.png / 3_special_sorts_linear.png\n");
    printf("  - pattern_comparison.png (All algorithms by pattern)\n");
}
