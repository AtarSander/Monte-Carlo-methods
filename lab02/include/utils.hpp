#ifndef LAB02_UTILS_HPP
#define LAB02_UTILS_HPP

#include <cstddef>
#include <memory>
#include <vector>

#include <gsl/gsl_rng.h>

enum class GslGeneratorKind {
    Ranlux,
    Randu
};

using GslRngPtr = std::unique_ptr<gsl_rng, void (*)(gsl_rng*)>;

GslRngPtr make_gsl_generator(GslGeneratorKind kind, unsigned long seed);
std::vector<double> generate_random_numbers(gsl_rng* generator, std::size_t count);
std::vector<std::size_t> build_logarithmic_checkpoints(std::size_t max_trials);
double estimate_hit_or_miss_stddev(double scale_factor, std::size_t trials, std::size_t hits);

#endif
