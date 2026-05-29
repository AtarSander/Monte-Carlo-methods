#include <cmath>
#include <iostream>
#include <numbers>
#include <stdexcept>
#include <string>
#include <vector>

#include <gsl/gsl_rng.h>

#include "utils.hpp"

namespace {

constexpr double kXMin = 0.0;
constexpr double kXMax = std::numbers::pi_v<double> / 2.0;
constexpr double kZMin = 0.0;
constexpr double kZMax = (kXMax * kXMax);
constexpr double kExactIntegral = 1.0;
constexpr std::size_t kInitialSamples = 100;

using lab04::ComparisonPoint;

double integrand(double x) {
    return std::sin(x);
}

double transformed_integrand(double z) {
    const double x = std::sqrt(z);
    return integrand(x) / (2.0 * x);
}

std::vector<std::size_t> build_checkpoints(std::size_t max_samples) {
    return lab04::build_logarithmic_checkpoints(max_samples, kInitialSamples);
}

std::vector<ComparisonPoint> estimate_standard_mc(gsl_rng* generator, std::size_t max_samples) {
    const auto checkpoints = build_checkpoints(max_samples);
    std::vector<ComparisonPoint> results;
    results.reserve(checkpoints.size());

    double sum_fx = 0.0;
    double sum_fx2 = 0.0;
    std::size_t checkpoint_index = 0;

    for (std::size_t sample = 1; sample <= max_samples; ++sample) {
        const double x = kXMin + (kXMax - kXMin) * gsl_rng_uniform(generator);
        const double fx = integrand(x);
        sum_fx += fx;
        sum_fx2 += fx * fx;

        if (checkpoint_index < checkpoints.size() && sample == checkpoints[checkpoint_index]) {
            const double n = static_cast<double>(sample);
            const double mean_fx = sum_fx / n;
            const double estimate = (kXMax - kXMin) * mean_fx;
            const double variance_estimator = (sum_fx2 - (sum_fx * sum_fx) / n) / (n * (n - 1.0));
            const double uncertainty = 2.0 * (kXMax - kXMin) * std::sqrt(variance_estimator);

            results.push_back({
                sample,
                "standard_mc",
                estimate,
                uncertainty,
                std::abs(estimate - kExactIntegral),
            });
            ++checkpoint_index;
        }
    }

    return results;
}

std::vector<ComparisonPoint> estimate_transformed_mc(gsl_rng* generator, std::size_t max_samples) {
    const auto checkpoints = build_checkpoints(max_samples);
    std::vector<ComparisonPoint> results;
    results.reserve(checkpoints.size());

    double sum_fz = 0.0;
    double sum_fz2 = 0.0;
    std::size_t checkpoint_index = 0;

    for (std::size_t sample = 1; sample <= max_samples; ++sample) {
        const double z = kZMin + (kZMax - kZMin) * gsl_rng_uniform_pos(generator);
        const double fz = transformed_integrand(z);
        sum_fz += fz;
        sum_fz2 += fz * fz;

        if (checkpoint_index < checkpoints.size() && sample == checkpoints[checkpoint_index]) {
            const double n = static_cast<double>(sample);
            const double mean_fz = sum_fz / n;
            const double estimate = (kZMax - kZMin) * mean_fz;
            const double variance_estimator = (sum_fz2 - (sum_fz * sum_fz) / n) / (n * (n - 1.0));
            const double uncertainty = 2.0 * (kZMax - kZMin) * std::sqrt(variance_estimator);

            results.push_back({
                sample,
                "mc_with_substitution",
                estimate,
                uncertainty,
                std::abs(estimate - kExactIntegral),
            });
            ++checkpoint_index;
        }
    }

    return results;
}

}  // namespace

int main(int argc, char* argv[]) {
    try {
        if (argc != 4) {
            std::cerr << "Usage: " << argv[0] << " <max_samples> <seed> <output_csv>\n";
            return 1;
        }

        const std::size_t max_samples = std::stoull(argv[1]);
        const unsigned long seed = std::stoul(argv[2]);
        const std::string output_csv = argv[3];

        if (max_samples < kInitialSamples) {
            throw std::invalid_argument("The number of samples must be at least 100.");
        }

        const auto standard_generator = lab04::make_generator(seed);
        const auto transformed_generator = lab04::make_generator(seed);

        auto standard_results = estimate_standard_mc(standard_generator.get(), max_samples);
        auto transformed_results = estimate_transformed_mc(transformed_generator.get(), max_samples);

        standard_results.insert(
            standard_results.end(), transformed_results.begin(), transformed_results.end());
        lab04::save_comparison_results(standard_results, output_csv);

        std::cout << "Saved results to " << output_csv << "\n";
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
