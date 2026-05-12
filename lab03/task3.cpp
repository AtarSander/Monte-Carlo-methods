#include <chrono>
#include <cmath>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include <gsl/gsl_rng.h>

#include "utils.hpp"

namespace {

using Clock = std::chrono::steady_clock;

double target_density(double x) {
    if (x < 0.0 || x > 2.0) {
        return 0.0;
    }

    const double shifted = x - 1.0;
    return (5.0 / 12.0) * (1.0 + shifted * shifted * shifted * shifted);
}

double sample_acceptance_rejection(gsl_rng* generator) {
    constexpr double kXMin = 0.0;
    constexpr double kXMax = 2.0;
    constexpr double kYMax = 5.0 / 6.0;

    while (true) {
        const double u1 = kXMin + (kXMax - kXMin) * gsl_rng_uniform(generator);
        const double u2 = kYMax * gsl_rng_uniform(generator);
        if (u2 <= target_density(u1)) {
            return u1;
        }
    }
}

double signed_fifth_root(double value) {
    const double magnitude = std::pow(std::abs(value), 1.0 / 5.0);
    return value < 0.0 ? -magnitude : magnitude;
}

double sample_superposition(gsl_rng* generator) {
    const double u1 = 2.0 * gsl_rng_uniform(generator);
    const double u2 = gsl_rng_uniform(generator);

    if (u2 < 5.0 / 6.0) {
        return u1;
    }

    return 1.0 + signed_fifth_root(u1 - 1.0);
}

using ScalarGenerator = double (*)(gsl_rng*);

std::vector<double> generate_values(gsl_rng* generator, std::size_t count, ScalarGenerator scalar_generator) {
    std::vector<double> values;
    values.reserve(count);

    for (std::size_t i = 0; i < count; ++i) {
        values.push_back(scalar_generator(generator));
    }

    return values;
}

long long measure_generation_time(gsl_rng* generator, std::size_t count, ScalarGenerator scalar_generator) {
    volatile double sink = 0.0;

    const auto start = Clock::now();
    for (std::size_t i = 0; i < count; ++i) {
        sink += scalar_generator(generator);
    }
    const auto end = Clock::now();

    (void)sink;
    return std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
}

}  // namespace

int main(int argc, char* argv[]) {
    try {
        if (argc != 5) {
            std::cerr << "Usage: " << argv[0] << " <sample_count> <timing_count> <seed> <output_dir>\n";
            return 1;
        }

        const std::size_t sample_count = std::stoull(argv[1]);
        const std::size_t timing_count = std::stoull(argv[2]);
        const unsigned long seed = std::stoul(argv[3]);
        const std::string output_dir = argv[4];

        if (sample_count == 0 || timing_count == 0) {
            throw std::invalid_argument("Counts must be positive.");
        }

        const auto rejection_generator = make_generator(seed);
        const auto superposition_generator = make_generator(seed);

        const auto rejection_values = generate_values(
            rejection_generator.get(), sample_count, sample_acceptance_rejection);
        const auto superposition_values = generate_values(
            superposition_generator.get(), sample_count, sample_superposition);

        save_values(rejection_values, output_dir + "/task3_acceptance_rejection.txt");
        save_values(superposition_values, output_dir + "/task3_superposition.txt");

        const auto rejection_timing_generator = make_generator(seed);
        const auto superposition_timing_generator = make_generator(seed);

        const std::vector<TimingResult> timings = {
            {"acceptance_rejection",
             measure_generation_time(rejection_timing_generator.get(), timing_count, sample_acceptance_rejection)},
            {"superposition",
             measure_generation_time(superposition_timing_generator.get(), timing_count, sample_superposition)},
        };

        save_timing_results(timings, output_dir + "/task3_timings.csv");
        std::cout << "Saved results to " << output_dir << "\n";

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
