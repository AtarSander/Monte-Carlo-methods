#pragma once

#include <cstddef>
#include <memory>
#include <string>
#include <vector>

#include <gsl/gsl_rng.h>

namespace lab04 {

using GslRngPtr = std::unique_ptr<gsl_rng, void (*)(gsl_rng*)>;

struct ComparisonPoint {
    std::size_t samples;
    std::string method;
    double estimate;
    double uncertainty;
    double abs_error;
};

GslRngPtr make_generator(unsigned long seed);
std::vector<std::size_t> build_logarithmic_checkpoints(std::size_t max_samples, std::size_t initial_samples);
void save_comparison_results(const std::vector<ComparisonPoint>& results, const std::string& filepath);

}  // namespace lab04
