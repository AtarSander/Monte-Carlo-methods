#include "utils.hpp"

#include <fstream>
#include <stdexcept>

GslRngPtr make_generator(unsigned long seed) {
    gsl_rng_env_setup();
    gsl_rng* generator = gsl_rng_alloc(gsl_rng_ranlux);
    if (generator == nullptr) {
        throw std::runtime_error("Failed to allocate GSL generator.");
    }

    gsl_rng_set(generator, seed);
    return GslRngPtr(generator, gsl_rng_free);
}


void save_values(const std::vector<double>& values, const std::string& filepath) {
    std::ofstream out(filepath);
    if (!out) {
        throw std::runtime_error("Cannot open output file: " + filepath);
    }

    for (double value : values) {
        out << value << "\n";
    }
}


void save_timing_results(const std::vector<TimingResult>& results, const std::string& filepath) {
    std::ofstream out(filepath);
    if (!out) {
        throw std::runtime_error("Cannot open output file: " + filepath);
    }

    out << "generator,time_us\n";
    for (const auto& result : results) {
        out << result.method << "," << result.microseconds << "\n";
    }
}
