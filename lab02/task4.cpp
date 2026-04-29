#include "utils.hpp"

#include <fstream>
#include <iostream>
#include <numbers>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

constexpr double kCubeVolume = 8.0;

struct SphereVolumeEstimateResult {
    std::size_t trials;
    double sphere_volume_estimate;
    double stddev;
};

std::vector<SphereVolumeEstimateResult> estimate_sphere_volume_hit_or_miss(gsl_rng* generator,
                                                                           std::size_t max_trials) {
    const auto checkpoints = build_logarithmic_checkpoints(max_trials);
    std::vector<SphereVolumeEstimateResult> results;
    results.reserve(checkpoints.size());

    std::size_t hits = 0;
    std::size_t checkpoint_index = 0;

    for (std::size_t trial = 1; trial <= max_trials; ++trial) {
        const double x = 2.0 * gsl_rng_uniform(generator) - 1.0;
        const double y = 2.0 * gsl_rng_uniform(generator) - 1.0;
        const double z = 2.0 * gsl_rng_uniform(generator) - 1.0;
        if (x * x + y * y + z * z <= 1.0) {
            ++hits;
        }

        if (checkpoint_index < checkpoints.size() && trial == checkpoints[checkpoint_index]) {
            const double volume_estimate = kCubeVolume * static_cast<double>(hits) / static_cast<double>(trial);
            results.push_back(
                {trial, volume_estimate, estimate_hit_or_miss_stddev(kCubeVolume, trial, hits)});
            ++checkpoint_index;
        }
    }

    return results;
}

void save_results(const std::vector<SphereVolumeEstimateResult>& results, const std::string& filename) {
    std::ofstream out(filename);
    if (!out) {
        throw std::runtime_error("Cannot open output file: " + filename);
    }

    out << "trials,sphere_volume_estimate,stddev\n";
    for (const auto& result : results) {
        out << result.trials << ","
            << result.sphere_volume_estimate << ","
            << result.stddev << "\n";
    }
}

}  // namespace

int main(int argc, char* argv[]) {
    try {
        if (argc != 4) {
            std::cerr << "Usage: " << argv[0] << " <max_trials> <seed> <output_csv>\n";
            return 1;
        }

        const std::size_t max_trials = std::stoull(argv[1]);
        const unsigned long seed = std::stoul(argv[2]);
        const std::string output_csv = argv[3];

        if (max_trials == 0) {
            throw std::invalid_argument("The number of trials must be positive.");
        }

        const auto generator = make_gsl_generator(GslGeneratorKind::Ranlux, seed);
        const auto results = estimate_sphere_volume_hit_or_miss(generator.get(), max_trials);
        save_results(results, output_csv);

        const double exact_volume = 4.0 * std::numbers::pi_v<double> / 3.0;
        const auto& last_result = results.back();
        std::cout << "Last estimate: " << last_result.sphere_volume_estimate
                  << " (exact value: " << exact_volume << ")\n";
        std::cout << "Saved results to " << output_csv << "\n";

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
