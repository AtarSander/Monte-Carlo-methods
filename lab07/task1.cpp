#include <cmath>
#include <fstream>
#include <iostream>
#include <random>
#include <string>
#include <vector>

#include "utils.hpp"

namespace {

constexpr double kCoverageProbability = 0.9545;
constexpr double kX1Mean = 0.0;
constexpr double kX1Stddev = 1.0;
constexpr double kX2Mean = 0.0;
constexpr double kX2Stddev = 2.0;

double model(double x1, double x2) {
    return x1 + x2;
}

std::vector<double> generate_samples(std::size_t samples, unsigned long seed) {
    std::mt19937_64 generator(seed);
    std::normal_distribution<double> x1_distribution(kX1Mean, kX1Stddev);
    std::normal_distribution<double> x2_distribution(kX2Mean, kX2Stddev);

    std::vector<double> values;
    values.reserve(samples);

    for (std::size_t index = 0; index < samples; ++index) {
        const double x1 = x1_distribution(generator);
        const double x2 = x2_distribution(generator);
        values.push_back(model(x1, x2));
    }

    return values;
}

lab07::ComparisonSummaryRow compute_gum_summary() {
    const double mean = kX1Mean + kX2Mean;
    const double stddev = std::sqrt((kX1Stddev * kX1Stddev) + (kX2Stddev * kX2Stddev));
    return {"gum", mean, stddev, mean - 2.0 * stddev, mean + 2.0 * stddev};
}

void save_samples(const std::vector<double>& samples, const std::string& output_csv) {
    std::ofstream out(output_csv);
    if (!out) {
        throw std::runtime_error("Cannot open output file: " + output_csv);
    }

    out << "y\n";
    for (const auto value : samples) {
        out << value << "\n";
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
        const auto mc_stats = lab07::compute_summary_stats(samples, kCoverageProbability);
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
