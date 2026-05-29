#include <algorithm>
#include <cmath>
#include <iostream>
#include <memory>
#include <numbers>
#include <stdexcept>
#include <string>
#include <vector>

#include <gsl/gsl_monte.h>
#include <gsl/gsl_monte_miser.h>
#include <gsl/gsl_monte_plain.h>
#include <gsl/gsl_monte_vegas.h>

#include "utils.hpp"

namespace {

constexpr double kLowerBound = -std::numbers::pi_v<double>;
constexpr double kUpperBound = std::numbers::pi_v<double>;
constexpr double kExactIntegral1D = 2.0 * std::numbers::pi_v<double>;
constexpr double kExactIntegral3D =
    8.0 * std::numbers::pi_v<double> * std::numbers::pi_v<double> * std::numbers::pi_v<double>;
constexpr std::size_t kInitialSamples = 100;

using PlainStatePtr = std::unique_ptr<gsl_monte_plain_state, void (*)(gsl_monte_plain_state*)>;
using MiserStatePtr = std::unique_ptr<gsl_monte_miser_state, void (*)(gsl_monte_miser_state*)>;
using VegasStatePtr = std::unique_ptr<gsl_monte_vegas_state, void (*)(gsl_monte_vegas_state*)>;
using lab04::ComparisonPoint;

double integrand_1d(double* x, std::size_t dim, void* /* params */) {
    if (dim != 1) {
        throw std::runtime_error("Invalid dimension for 1D integrand.");
    }

    return 1.0 + std::cos(x[0]);
}

double integrand_3d(double* x, std::size_t dim, void* /* params */) {
    if (dim != 3) {
        throw std::runtime_error("Invalid dimension for 3D integrand.");
    }

    return (1.0 + std::cos(x[0])) * (1.0 + std::cos(x[1])) * (1.0 + std::cos(x[2]));
}

PlainStatePtr make_plain_state(std::size_t dimension) {
    gsl_monte_plain_state* state = gsl_monte_plain_alloc(dimension);
    if (state == nullptr) {
        throw std::runtime_error("Failed to allocate plain Monte Carlo state.");
    }

    return PlainStatePtr(state, gsl_monte_plain_free);
}

MiserStatePtr make_miser_state(std::size_t dimension) {
    gsl_monte_miser_state* state = gsl_monte_miser_alloc(dimension);
    if (state == nullptr) {
        throw std::runtime_error("Failed to allocate MISER Monte Carlo state.");
    }

    return MiserStatePtr(state, gsl_monte_miser_free);
}

VegasStatePtr make_vegas_state(std::size_t dimension) {
    gsl_monte_vegas_state* state = gsl_monte_vegas_alloc(dimension);
    if (state == nullptr) {
        throw std::runtime_error("Failed to allocate VEGAS Monte Carlo state.");
    }

    return VegasStatePtr(state, gsl_monte_vegas_free);
}

std::vector<std::size_t> build_checkpoints(std::size_t max_samples) {
    return lab04::build_logarithmic_checkpoints(max_samples, kInitialSamples);
}

ComparisonPoint run_plain(
    gsl_monte_function* function,
    const std::vector<double>& lower_bounds,
    const std::vector<double>& upper_bounds,
    std::size_t samples,
    double exact_value,
    gsl_rng* generator) {
    const auto state = make_plain_state(function->dim);
    gsl_monte_plain_init(state.get());

    double estimate = 0.0;
    double error = 0.0;
    gsl_monte_plain_integrate(
        function,
        lower_bounds.data(),
        upper_bounds.data(),
        function->dim,
        samples,
        generator,
        state.get(),
        &estimate,
        &error);

    return {
        samples,
        "plain",
        estimate,
        2.0 * error,
        std::abs(estimate - exact_value),
    };
}

ComparisonPoint run_miser(
    gsl_monte_function* function,
    const std::vector<double>& lower_bounds,
    const std::vector<double>& upper_bounds,
    std::size_t samples,
    double exact_value,
    gsl_rng* generator) {
    const auto state = make_miser_state(function->dim);
    gsl_monte_miser_init(state.get());

    double estimate = 0.0;
    double error = 0.0;
    gsl_monte_miser_integrate(
        function,
        lower_bounds.data(),
        upper_bounds.data(),
        function->dim,
        samples,
        generator,
        state.get(),
        &estimate,
        &error);

    return {
        samples,
        "miser",
        estimate,
        2.0 * error,
        std::abs(estimate - exact_value),
    };
}

ComparisonPoint run_vegas(
    gsl_monte_function* function,
    const std::vector<double>& lower_bounds,
    const std::vector<double>& upper_bounds,
    std::size_t samples,
    double exact_value,
    gsl_rng* generator) {
    const auto state = make_vegas_state(function->dim);
    gsl_monte_vegas_init(state.get());

    const std::size_t warmup_samples = std::max<std::size_t>(1000, samples / 5);
    auto mutable_lower_bounds = lower_bounds;
    auto mutable_upper_bounds = upper_bounds;
    double estimate = 0.0;
    double error = 0.0;

    gsl_monte_vegas_integrate(
        function,
        mutable_lower_bounds.data(),
        mutable_upper_bounds.data(),
        function->dim,
        warmup_samples,
        generator,
        state.get(),
        &estimate,
        &error);

    gsl_monte_vegas_integrate(
        function,
        mutable_lower_bounds.data(),
        mutable_upper_bounds.data(),
        function->dim,
        samples,
        generator,
        state.get(),
        &estimate,
        &error);

    return {
        samples,
        "vegas",
        estimate,
        2.0 * error,
        std::abs(estimate - exact_value),
    };
}

std::vector<ComparisonPoint> evaluate_integral(
    gsl_monte_function* function,
    const std::vector<double>& lower_bounds,
    const std::vector<double>& upper_bounds,
    std::size_t max_samples,
    unsigned long seed,
    double exact_value) {
    const auto checkpoints = build_checkpoints(max_samples);
    std::vector<ComparisonPoint> results;
    results.reserve(checkpoints.size() * 3);

    const auto plain_generator = lab04::make_generator(seed);
    const auto miser_generator = lab04::make_generator(seed + 1);
    const auto vegas_generator = lab04::make_generator(seed + 2);

    for (const auto samples : checkpoints) {
        results.push_back(
            run_plain(function, lower_bounds, upper_bounds, samples, exact_value, plain_generator.get()));
        results.push_back(
            run_miser(function, lower_bounds, upper_bounds, samples, exact_value, miser_generator.get()));
        results.push_back(
            run_vegas(function, lower_bounds, upper_bounds, samples, exact_value, vegas_generator.get()));
    }

    return results;
}

}  // namespace

int main(int argc, char* argv[]) {
    try {
        if (argc != 5) {
            std::cerr << "Usage: " << argv[0]
                      << " <max_samples> <seed> <output_1d_csv> <output_3d_csv>\n";
            return 1;
        }

        const std::size_t max_samples = std::stoull(argv[1]);
        const unsigned long seed = std::stoul(argv[2]);
        const std::string output_1d_csv = argv[3];
        const std::string output_3d_csv = argv[4];

        if (max_samples < kInitialSamples) {
            throw std::invalid_argument("The number of samples must be at least 100.");
        }

        gsl_monte_function function_1d{&integrand_1d, 1, nullptr};
        gsl_monte_function function_3d{&integrand_3d, 3, nullptr};

        const std::vector<double> lower_1d = {kLowerBound};
        const std::vector<double> upper_1d = {kUpperBound};
        const std::vector<double> lower_3d = {kLowerBound, kLowerBound, kLowerBound};
        const std::vector<double> upper_3d = {kUpperBound, kUpperBound, kUpperBound};

        const auto results_1d =
            evaluate_integral(&function_1d, lower_1d, upper_1d, max_samples, seed, kExactIntegral1D);
        const auto results_3d =
            evaluate_integral(&function_3d, lower_3d, upper_3d, max_samples, seed + 100, kExactIntegral3D);

        lab04::save_comparison_results(results_1d, output_1d_csv);
        lab04::save_comparison_results(results_3d, output_3d_csv);

        std::cout << "Saved 1D results to " << output_1d_csv << "\n";
        std::cout << "Saved 3D results to " << output_3d_csv << "\n";
        std::cout << "Exact 1D integral value: " << kExactIntegral1D << "\n";
        std::cout << "Exact 3D integral value: " << kExactIntegral3D << "\n";
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
