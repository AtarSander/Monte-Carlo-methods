#include <cmath>
#include <fstream>
#include <iostream>
#include <random>
#include <string>
#include <vector>

#include "utils.hpp"

namespace {

constexpr double kCoverageProbability = 0.9545;
constexpr double kX1Min = 0.0;
constexpr double kX1Max = 4.0;
constexpr double kX2Min = 5.0;
constexpr double kX2Max = 6.0;

struct SampleRow {
    double x1;
    double x2;
    double y;
};

double model(double x1, double x2) {
    return x1 + x2;
}

std::vector<SampleRow> generate_samples(std::size_t samples, unsigned long seed) {
    std::mt19937_64 generator(seed);
    std::uniform_real_distribution<double> x1_distribution(kX1Min, kX1Max);
    std::uniform_real_distribution<double> x2_distribution(kX2Min, kX2Max);

    std::vector<SampleRow> values;
    values.reserve(samples);

    for (std::size_t index = 0; index < samples; ++index) {
        const double x1 = x1_distribution(generator);
        const double x2 = x2_distribution(generator);
        values.push_back({x1, x2, model(x1, x2)});
    }

    return values;
}

lab07::ComparisonSummaryRow compute_gum_summary() {
    const double x1_mean = (kX1Min + kX1Max) / 2.0;
    const double x2_mean = (kX2Min + kX2Max) / 2.0;
    const double x1_variance = std::pow(kX1Max - kX1Min, 2) / 12.0;
    const double x2_variance = std::pow(kX2Max - kX2Min, 2) / 12.0;

    const double mean = x1_mean + x2_mean;
    const double stddev = std::sqrt(x1_variance + x2_variance);
    return {"gum", mean, stddev, mean - 2.0 * stddev, mean + 2.0 * stddev};
}

void save_samples(const std::vector<SampleRow>& samples, const std::string& output_csv) {
    std::ofstream out(output_csv);
    if (!out) {
        throw std::runtime_error("Cannot open output file: " + output_csv);
    }

    out << "x1,x2,y\n";
    for (const auto& sample : samples) {
        out << sample.x1 << "," << sample.x2 << "," << sample.y << "\n";
    }
}

}  // namespace

int main(int argc, char* argv[]) {
    try {
        if (argc != 5) {
            std::cerr << "Usage: " << argv[0] << " <samples> <seed> <summary_csv> <samples_csv>\n";
            return 1;
        }

        const std::size_t sample_count = std::stoull(argv[1]);
        const unsigned long seed = std::stoul(argv[2]);
        const std::string summary_csv = argv[3];
        const std::string samples_csv = argv[4];

        lab07::validate_sample_count(sample_count);

        const auto samples = generate_samples(sample_count, seed);
        std::vector<double> y_values;
        y_values.reserve(samples.size());
        for (const auto& sample : samples) {
            y_values.push_back(sample.y);
        }
        const auto mc_stats = lab07::compute_summary_stats(y_values, kCoverageProbability);
        const lab07::ComparisonSummaryRow mc_summary{
            "mc", mc_stats.mean, mc_stats.stddev, mc_stats.interval_min, mc_stats.interval_max};
        const auto gum_summary = compute_gum_summary();

        lab07::save_comparison_summary({mc_summary, gum_summary}, summary_csv);
        save_samples(samples, samples_csv);

        std::cout << "Saved summary to " << summary_csv << "\n";
        std::cout << "Saved Monte Carlo samples to " << samples_csv << "\n";
        std::cout << "MC mean = " << mc_summary.mean << ", MC stddev = " << mc_summary.stddev << "\n";
        std::cout << "GUM mean = " << gum_summary.mean << ", GUM stddev = " << gum_summary.stddev << "\n";
    } catch (const std::exception& error) {
        std::cerr << "Error: " << error.what() << "\n";
        return 1;
    }

    return 0;
}
