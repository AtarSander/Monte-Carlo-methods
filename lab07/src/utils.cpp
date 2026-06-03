#include "utils.hpp"

#include <algorithm>
#include <cmath>
#include <fstream>
#include <stdexcept>

namespace lab07 {

void validate_sample_count(std::size_t sample_count) {
    if (sample_count < 2) {
        throw std::invalid_argument("The number of Monte Carlo samples must be at least 2.");
    }
}

SummaryStats compute_summary_stats(const std::vector<double>& values, double coverage_probability) {
    validate_sample_count(values.size());

    double sum = 0.0;
    double sum_squared = 0.0;
    for (const auto value : values) {
        sum += value;
        sum_squared += value * value;
    }

    const double sample_count = static_cast<double>(values.size());
    const double mean = sum / sample_count;
    const double variance = (sum_squared - (sum * sum) / sample_count) / (sample_count - 1.0);
    const double stddev = std::sqrt(variance);

    std::vector<double> sorted_values = values;
    std::sort(sorted_values.begin(), sorted_values.end());

    const std::size_t q = static_cast<std::size_t>(std::llround(coverage_probability * sample_count));
    const std::size_t r = (values.size() - q) / 2;

    return {
        mean,
        stddev,
        sorted_values[r],
        sorted_values[r + q - 1],
    };
}

void save_comparison_summary(
    const std::vector<ComparisonSummaryRow>& rows, const std::string& output_csv) {
    std::ofstream out(output_csv);
    if (!out) {
        throw std::runtime_error("Cannot open output file: " + output_csv);
    }

    out << "method,mean,stddev,interval_min,interval_max\n";
    for (const auto& row : rows) {
        out << row.method << ","
            << row.mean << ","
            << row.stddev << ","
            << row.interval_min << ","
            << row.interval_max << "\n";
    }
}

}  // namespace lab07
