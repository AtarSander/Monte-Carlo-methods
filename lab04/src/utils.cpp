#include "utils.hpp"

#include <fstream>
#include <stdexcept>

#include <gsl/gsl_rng.h>

namespace lab04 {

GslRngPtr make_generator(unsigned long seed) {
    gsl_rng_env_setup();
    gsl_rng* generator = gsl_rng_alloc(gsl_rng_ranlux);
    if (generator == nullptr) {
        throw std::runtime_error("Failed to allocate GSL generator.");
    }

    gsl_rng_set(generator, seed);
    return GslRngPtr(generator, gsl_rng_free);
}

std::vector<std::size_t> build_logarithmic_checkpoints(std::size_t max_samples, std::size_t initial_samples) {
    std::vector<std::size_t> checkpoints;
    for (std::size_t scale = initial_samples; scale <= max_samples; scale *= 10) {
        for (const std::size_t multiplier : {1ULL, 2ULL, 5ULL}) {
            const std::size_t checkpoint = multiplier * scale;
            if (checkpoint < initial_samples || checkpoint > max_samples) {
                continue;
            }

            if (checkpoints.empty() || checkpoints.back() != checkpoint) {
                checkpoints.push_back(checkpoint);
            }
        }

        if (scale > max_samples / 10) {
            break;
        }
    }

    if (checkpoints.empty() || checkpoints.back() != max_samples) {
        checkpoints.push_back(max_samples);
    }

    return checkpoints;
}

void save_comparison_results(const std::vector<ComparisonPoint>& results, const std::string& filepath) {
    std::ofstream out(filepath);
    if (!out) {
        throw std::runtime_error("Cannot open output file: " + filepath);
    }

    out << "samples,method,estimate,uncertainty,abs_error,errorbar\n";
    for (const auto& result : results) {
        out << result.samples << ","
            << result.method << ","
            << result.estimate << ","
            << result.uncertainty << ","
            << result.abs_error << ","
            << result.uncertainty / 2.0 << "\n";
    }
}

}  // namespace lab04
