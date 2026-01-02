#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "benchmark.h"
#include "visualizer.h"

static void print_usage(const char *program_name) {
    printf("Usage: %s [options]\n", program_name);
    printf("  --plot-only            Regenerate charts from existing CSV files\n");
    printf("  --no-large-sizes       Limit efficient algorithms to <= 1M elements\n");
    printf("  --include-large-sizes  Explicitly enable extended sizes (default)\n");
    printf("  --stats-only           Run only the stats benchmark (comparisons/swaps)\n");
}

int main(int argc, char *argv[]) {
    printf("=== Sorting Algorithm Performance Benchmark ===\n\n");

    bool plot_only = false;
    bool include_large_sizes = true;
    bool stats_only = false;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--plot-only") == 0) {
            plot_only = true;
        } else if (strcmp(argv[i], "--include-large-sizes") == 0) {
            include_large_sizes = true;
        } else if (strcmp(argv[i], "--no-large-sizes") == 0) {
            include_large_sizes = false;
        } else if (strcmp(argv[i], "--stats-only") == 0) {
            stats_only = true;
        } else if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
            print_usage(argv[0]);
            return 0;
        } else {
            printf("Unknown option: %s\n\n", argv[i]);
            print_usage(argv[0]);
            return 1;
        }
    }

    if (stats_only) {
        printf("Running stats benchmark only...\n\n");
        run_stats_benchmark();
        printf("\nStats benchmark completed! Check results/stats_benchmark.csv\n");
        return 0;
    }

    if (plot_only) {
        printf("Running in plot-only mode (using existing CSV data)...\n\n");
        generate_all_plots();
        printf("\nVisualization completed! Check results/ folder.\n");
        return 0;
    }

    if (include_large_sizes) {
        printf("Including extended input sizes (may take hours and require >1 GB RAM).\n\n");
    } else {
        printf("Extended input sizes disabled (<= 1M elements).\n\n");
    }

    run_all_benchmarks(include_large_sizes);

    // Generate visualization graphs
    generate_all_plots();

    printf("\nBenchmark completed! Check results/ folder.\n");
    return 0;
}
