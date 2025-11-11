#ifndef VISUALIZER_H
#define VISUALIZER_H

/**
 * Plot performance comparison by input size
 * X-axis: Input size (log scale)
 * Y-axis: Execution time (log scale)
 * Shows how each algorithm scales with input size
 */
void plot_size_comparison(const char *output_file);

/**
 * Plot performance comparison by data pattern
 * Bar chart showing best/average/worst case performance
 * Highlights which patterns affect which algorithms
 */
void plot_pattern_comparison(const char *output_file);

/**
 * Generate all visualization graphs
 * Creates PNG files in results/ directory
 */
void generate_all_plots();

#endif
