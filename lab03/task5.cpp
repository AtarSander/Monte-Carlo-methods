#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include <gsl/gsl_randist.h>

#include "utils.hpp"

namespace {

std::vector<double> generate_cauchy_samples(gsl_rng* generator, std::size_t count) {
    std::vector<double> values;
    values.reserve(count);

    for (std::size_t i = 0; i < count; ++i) {
        const double x = gsl_ran_ugaussian(generator);
        const double y = gsl_ran_ugaussian(generator);
        values.push_back(x / y);
    }

    return values;
}

std::vector<double> generate_triangular_samples(gsl_rng* generator, std::size_t count, double a, double b) {
    const double midpoint = (a + b) / 2.0;
    std::vector<double> values;
    values.reserve(count);

    for (std::size_t i = 0; i < count; ++i) {
        const double x = a + (midpoint - a) * gsl_rng_uniform(generator);
        const double y = a + (midpoint - a) * gsl_rng_uniform(generator);
        values.push_back(x + y);
    }

    return values;
}

}  // namespace

int main(int argc, char* argv[]) {
    try {
        if (argc != 3) {
            std::cerr << "Usage: " << argv[0] << " <sample_count> <output_dir>\n";
            return 1;
        }

        const std::size_t sample_count = std::stoull(argv[1]);
        const std::string output_dir = argv[2];
        if (sample_count == 0) {
            throw std::invalid_argument("Sample count must be positive.");
        }

        constexpr unsigned long seed = 1234;
        constexpr double triangular_a = 0.0;
        constexpr double triangular_b = 2.0;

        const auto cauchy_generator = make_generator(seed);
        const auto triangular_generator = make_generator(seed);

        const auto cauchy_values = generate_cauchy_samples(cauchy_generator.get(), sample_count);
        const auto triangular_values = generate_triangular_samples(
            triangular_generator.get(), sample_count, triangular_a, triangular_b);

        save_values(cauchy_values, output_dir + "/task5_cauchy.txt");
        save_values(triangular_values, output_dir + "/task5_triangular.txt");

        std::cout << "Saved results to " << output_dir << "\n";
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
