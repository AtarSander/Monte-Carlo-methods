#include "utils.hpp"

#include <cmath>
#include <stdexcept>

namespace {

gsl_rng* create_gsl_generator(GslGeneratorKind kind, unsigned long seed) {
    gsl_rng_env_setup();

    const gsl_rng_type* type = nullptr;
    switch (kind) {
    case GslGeneratorKind::Ranlux:
        type = gsl_rng_ranlux;
        break;
    case GslGeneratorKind::Randu:
        type = gsl_rng_randu;
        break;
    }

    gsl_rng* generator = gsl_rng_alloc(type);
    if (generator == nullptr) {
        throw std::runtime_error("Failed to allocate GSL generator.");
    }

    gsl_rng_set(generator, seed);
    return generator;
}

}  // namespace


GslRngPtr make_gsl_generator(GslGeneratorKind kind, unsigned long seed) {
    return GslRngPtr(create_gsl_generator(kind, seed), gsl_rng_free);
}


std::vector<double> generate_random_numbers(gsl_rng* generator, std::size_t count) {
    std::vector<double> numbers;
    numbers.reserve(count);

    for (std::size_t i = 0; i < count; ++i) {
        numbers.push_back(gsl_rng_uniform(generator));
    }

    return numbers;
}


std::vector<std::size_t> build_logarithmic_checkpoints(std::size_t max_trials) {
    std::vector<std::size_t> checkpoints;
    for (std::size_t scale = 10; scale <= max_trials; scale *= 10) {
        for (std::size_t multiplier : {2ULL, 5ULL, 10ULL}) {
            const std::size_t checkpoint = multiplier * scale;
            if (checkpoint > max_trials) {
                continue;
            }
            checkpoints.push_back(checkpoint);
        }

        if (scale > max_trials / 10) {
            break;
        }
    }

    if (checkpoints.empty() || checkpoints.back() != max_trials) {
        checkpoints.push_back(max_trials);
    }

    return checkpoints;
}


double estimate_hit_or_miss_stddev(double scale_factor, std::size_t trials, std::size_t hits) {
    const double hit_ratio = static_cast<double>(hits) / static_cast<double>(trials);
    return scale_factor * std::sqrt((hit_ratio * (1.0 - hit_ratio)) / static_cast<double>(trials));
}
