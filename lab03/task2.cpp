#include <chrono>
#include <cmath>
#include <iostream>
#include <numbers>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include <gsl/gsl_rng.h>

#include "utils.hpp"

namespace {

using Clock = std::chrono::steady_clock;

std::pair<double, double> box_muller_pair(gsl_rng* generator) {
    const double u1 = gsl_rng_uniform_pos(generator);
    const double u2 = gsl_rng_uniform(generator);
    const double radius = std::sqrt(-2.0 * std::log(u1));
    const double angle = 2.0 * std::numbers::pi_v<double> * u2;

    return {radius * std::cos(angle), radius * std::sin(angle)};
}

std::pair<double, double> marsaglia_bray_pair(gsl_rng* generator) {
    while (true) {
        const double u1 = 2.0 * gsl_rng_uniform(generator) - 1.0;
        const double u2 = 2.0 * gsl_rng_uniform(generator) - 1.0;
        const double b = u1 * u1 + u2 * u2;

        if (b == 0.0 || b > 1.0) {
            continue;
        }

        const double factor = std::sqrt(-2.0 * std::log(b) / b);
        return {u1 * factor, u2 * factor};
    }
}

using PairGenerator = std::pair<double, double> (*)(gsl_rng*);

void generate_components(gsl_rng* generator,
                         std::size_t pair_count,
                         PairGenerator pair_generator,
                         std::vector<double>& x_values,
                         std::vector<double>& y_values) {
    x_values.clear();
    y_values.clear();
    x_values.reserve(pair_count);
    y_values.reserve(pair_count);

    for (std::size_t i = 0; i < pair_count; ++i) {
        const auto [x, y] = pair_generator(generator);
        x_values.push_back(x);
        y_values.push_back(y);
    }
}

long long measure_generation_time(gsl_rng* generator, std::size_t pair_count, PairGenerator pair_generator) {
    volatile double sink = 0.0;

    const auto start = Clock::now();
    for (std::size_t i = 0; i < pair_count; ++i) {
        const auto [x, y] = pair_generator(generator);
        sink += x + y;
    }
    const auto end = Clock::now();

    (void)sink;
    return std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
}

}  // namespace

int main(int argc, char* argv[]) {
    try {
        if (argc != 5) {
            std::cerr << "Usage: " << argv[0] << " <sample_pair_count> <timing_pair_count> <seed> <output_dir>\n";
            return 1;
        }

        const std::size_t sample_pair_count = std::stoull(argv[1]);
        const std::size_t timing_pair_count = std::stoull(argv[2]);
        const unsigned long seed = std::stoul(argv[3]);
        const std::string output_dir = argv[4];

        if (sample_pair_count == 0 || timing_pair_count == 0) {
            throw std::invalid_argument("Pair counts must be positive.");
        }

        const auto box_muller_sample_generator = make_generator(seed);
        const auto marsaglia_bray_sample_generator = make_generator(seed);
        std::vector<double> box_muller_x;
        std::vector<double> box_muller_y;
        std::vector<double> marsaglia_bray_x;
        std::vector<double> marsaglia_bray_y;

        generate_components(
            box_muller_sample_generator.get(), sample_pair_count, box_muller_pair, box_muller_x, box_muller_y);
        generate_components(
            marsaglia_bray_sample_generator.get(), sample_pair_count, marsaglia_bray_pair, marsaglia_bray_x, marsaglia_bray_y);

        save_values(box_muller_x, output_dir + "/task2_box_muller_x.txt");
        save_values(box_muller_y, output_dir + "/task2_box_muller_y.txt");
        save_values(marsaglia_bray_x, output_dir + "/task2_marsaglia_bray_x.txt");
        save_values(marsaglia_bray_y, output_dir + "/task2_marsaglia_bray_y.txt");

        const auto box_muller_timing_generator = make_generator(seed);
        const auto marsaglia_bray_timing_generator = make_generator(seed);

        const std::vector<TimingResult> timings = {
            {"box_muller", measure_generation_time(box_muller_timing_generator.get(), timing_pair_count, box_muller_pair)},
            {"marsaglia_bray", measure_generation_time(marsaglia_bray_timing_generator.get(), timing_pair_count, marsaglia_bray_pair)},
        };

        save_timing_results(timings, output_dir + "/task2_timings.csv");
        std::cout << "Saved results to " << output_dir << "\n";

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
