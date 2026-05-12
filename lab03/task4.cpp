#include <array>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include <gsl/gsl_randist.h>

#include "utils.hpp"

namespace {

std::vector<double> generate_laplace_samples(gsl_rng* generator, std::size_t count, double scale) {
    std::vector<double> values;
    values.reserve(count);
    for (std::size_t i = 0; i < count; ++i) {
        values.push_back(gsl_ran_laplace(generator, scale));
    }
    return values;
}

std::vector<double> generate_pareto_samples(gsl_rng* generator, std::size_t count, double shape, double scale) {
    std::vector<double> values;
    values.reserve(count);
    for (std::size_t i = 0; i < count; ++i) {
        values.push_back(gsl_ran_pareto(generator, shape, scale));
    }
    return values;
}

std::vector<double> generate_gamma_samples(gsl_rng* generator, std::size_t count, double shape, double scale) {
    std::vector<double> values;
    values.reserve(count);
    for (std::size_t i = 0; i < count; ++i) {
        values.push_back(gsl_ran_gamma(generator, shape, scale));
    }
    return values;
}

std::vector<std::array<double, 3>> generate_dirichlet_samples(gsl_rng* generator,
                                                              std::size_t count,
                                                              const std::array<double, 3>& alpha) {
    std::vector<std::array<double, 3>> values;
    values.reserve(count);

    for (std::size_t i = 0; i < count; ++i) {
        std::array<double, 3> sample {};
        gsl_ran_dirichlet(generator, alpha.size(), alpha.data(), sample.data());
        values.push_back(sample);
    }

    return values;
}

void save_dirichlet_values(const std::vector<std::array<double, 3>>& values, const std::string& filepath) {
    std::ofstream out(filepath);
    if (!out) {
        throw std::runtime_error("Cannot open output file: " + filepath);
    }

    out << "theta1,theta2,theta3\n";
    for (const auto& sample : values) {
        out << sample[0] << "," << sample[1] << "," << sample[2] << "\n";
    }
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
        constexpr double laplace_scale = 1.0;
        constexpr double pareto_shape = 3.0;
        constexpr double pareto_scale = 1.0;
        constexpr double gamma_shape = 2.0;
        constexpr double gamma_scale = 2.0;
        constexpr std::array<double, 3> dirichlet_alpha = {2.0, 3.0, 5.0};

        const auto laplace_generator = make_generator(seed);
        const auto pareto_generator = make_generator(seed);
        const auto gamma_generator = make_generator(seed);
        const auto dirichlet_generator = make_generator(seed);

        const auto laplace_values = generate_laplace_samples(laplace_generator.get(), sample_count, laplace_scale);
        const auto pareto_values = generate_pareto_samples(
            pareto_generator.get(), sample_count, pareto_shape, pareto_scale);
        const auto gamma_values = generate_gamma_samples(gamma_generator.get(), sample_count, gamma_shape, gamma_scale);
        const auto dirichlet_values = generate_dirichlet_samples(
            dirichlet_generator.get(), sample_count, dirichlet_alpha);

        save_values(laplace_values, output_dir + "/task4_laplace.txt");
        save_values(pareto_values, output_dir + "/task4_pareto.txt");
        save_values(gamma_values, output_dir + "/task4_gamma.txt");
        save_dirichlet_values(dirichlet_values, output_dir + "/task4_dirichlet.csv");

        std::cout << "Saved results to " << output_dir << "\n";
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
