#pragma once

#include <cstddef>
#include <string>
#include <vector>

namespace lab07 {

struct SummaryStats {
    double mean;
    double stddev;
    double interval_min;
    double interval_max;
};

struct ComparisonSummaryRow {
    std::string method;
    double mean;
    double stddev;
    double interval_min;
    double interval_max;
};

void validate_sample_count(std::size_t sample_count);
SummaryStats compute_summary_stats(const std::vector<double>& values, double coverage_probability);
void save_comparison_summary(
    const std::vector<ComparisonSummaryRow>& rows, const std::string& output_csv);

}  // namespace lab07
